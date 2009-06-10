/*****************************************
   Copyright 2009 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <crush/reutils.h>

#ifdef HAVE_LIBPCRE
int crush_re_make_flags(const char const *modifiers, int *global) {
  int flags = 0;
  int i, len;

  len = strlen(modifiers);
  for (i=0; i < len; i++) {
    switch(modifiers[i]) {
      case 'g': *global = 1; break;
      case 'i': flags |= PCRE_CASELESS; break;
    }
  }
  return flags;
}

/* Worker function used by crush_re_substitute().  It handles the part of the
 * substitution of the matched portion of the subject string, including
 * variable expansion. */
static char * _crush_re_expand_subst(const char *subject,
                                     const char *substitution,
                                     int *ovector, int n_pairs,
                                     char **target, size_t *target_sz,
                                     size_t *target_offset) {
  int is_escaped = 0;
  unsigned int capt_var;
  char *p;

  assert(*target);
  assert(*target_sz > 0);

  p = (char *) substitution;
  while (*p) {
    if (*p == '\\') {
      is_escaped = 1;
      p++;
    }

    if (! is_escaped && *p == '$') {
      int has_brace = 0;
      int good_variable;

      p++;
      has_brace = (*p == '{');

      if (has_brace) {
        char closing_brace;
        good_variable = sscanf(p++, "{%u%c", &capt_var, &closing_brace);
        if (closing_brace != '}')
          good_variable = 0;
      } else {
        good_variable = sscanf(p, "%u", &capt_var);
      }

      if (good_variable > 0) {
        /* Scan past the variable in the subject string. */
        while (isdigit(*p))
          p++;
        if (has_brace && *p == '}')
          p++;

        /* Copy the captured value into the target string, resizing the
         * target and retrying as necessary.  Treat out-of-bounds variables
         * as empty strings. */
        for (;;) {
          int var_val_len = pcre_copy_substring(subject, ovector, n_pairs,
                                                capt_var,
                                                *target + *target_offset,
                                                *target_sz - *target_offset);
          if (var_val_len == PCRE_ERROR_NOMEMORY) {
            int add_size = ovector[capt_var*2 + 1] - ovector[capt_var*2] + 32;
            char *tmp = realloc(*target, *target_sz + add_size);
            if (! tmp)
              return NULL;
            *target = tmp;
            *target_sz += add_size;
            continue;
          }
          if (var_val_len == PCRE_ERROR_NOSUBSTRING)
            var_val_len = 0;
          assert(var_val_len >= 0);
          *target_offset += var_val_len;
          break;
        }
      } else {
        /* Not a valid captured variable.  E.g. '${}' or '$non_digits'.
         * Not 100% Perl-compatible, but we'll just stuff the characters that
         * looked like a variable back into the target. */
        (*target)[(*target_offset)++] = '$';
        if (has_brace)
          (*target)[(*target_offset)++] = '{';
      }
    } else {
      /* TODO(jhinds): Expand escape sequences as Perl would do.  For literals
       * (e.g. '\$' or '\\') it already works, but for metachars like '\t', it
       * does not. */
      if (*target_offset >= *target_sz) {
        char *tmp = realloc(*target, *target_sz + 32);
        if (! tmp)
          return NULL;
        *target = tmp;
        *target_sz += 32;
      }
      (*target)[(*target_offset)++] = *p++;
      is_escaped = 0;
    }
    (*target)[*target_offset] = '\0';
  }
  return *target;
}

char * crush_re_substitute(pcre *re, pcre_extra *re_extra,
                           const char *subject,
                           const char *substitution,
                           char **target, size_t *target_sz,
                           int subst_globally) {
  int ovector[36];
  int prev_match_stop = 0;
  int n_pairs;
  int subject_len = strlen(subject);
  size_t target_offset = 0;

  if (! *target || *target_sz == 0) {
    *target_sz = subject_len;
    *target = malloc(*target_sz);
    if (! *target) {
      *target_sz = 0;
      return NULL;
    }
  }
  if (*target_sz < subject_len) {
    char *tmp = realloc(*target, subject_len);
    if (! tmp)
      return NULL;
    *target = tmp;
    *target_sz = subject_len;
  }

  do {
    memset(ovector, 0, sizeof(int) * 36);
    n_pairs = pcre_exec(re, re_extra,
                        subject + prev_match_stop,
                        subject_len - prev_match_stop,
                        0, 0, ovector, 36);

    if (ovector[0] == -1) {
      if (*target_sz - target_offset < subject_len - prev_match_stop) {
        int add_size = subject_len - prev_match_stop + 4;
        char *tmp = realloc(*target, *target_sz + add_size);
        if (! tmp)
          return NULL;
        *target = tmp;
        *target_sz += add_size;
      }
      strcpy(*target + target_offset, subject + prev_match_stop);
      break;
    }

    /* Copy everything between matches into the target. */
    strncpy(*target + target_offset, subject + prev_match_stop, ovector[0]);
    target_offset += ovector[0];
    (*target)[target_offset] = '\0';

    if (! _crush_re_expand_subst(subject + prev_match_stop,
                                 substitution, ovector, n_pairs,
                                 target, target_sz, &target_offset))
      return NULL;

    prev_match_stop += ovector[1];

    if (0) {
      int i;
      for (i=0; i < 20; i+=2)
        printf("(%d, %d) ", ovector[i], ovector[i+1]);
      printf("\nprev_match_stop = %d\ntarget_offset = %d\n",
             prev_match_stop, target_offset);
      printf("target = %s\n", *target);
    }
  } while (subst_globally && ovector[1] > -1);

  if (! subst_globally) {
    strcpy(*target + target_offset, subject + prev_match_stop);
  }

  return *target;
}

#endif /* HAVE_LIBPCRE */

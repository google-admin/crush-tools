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
#include <crush/general.h>
#include <crush/reutils.h>

#if HAVE_LIBPCRE && HAVE_PCRE_H
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


int crush_resubst_compile(const char *subst_pattern,
                          struct crush_resubst_elem **compiled_subst,
                          size_t *compiled_subst_sz) {
  int n_elems = 0;
  int is_escaped = 0;
  unsigned int capt_var;
  char *p;
  size_t needed_sz = strlen(subst_pattern) *
                     sizeof(struct crush_resubst_elem);

  if (! *compiled_subst || ! *compiled_subst_sz) {
    *compiled_subst = xmalloc(needed_sz);
    *compiled_subst_sz = needed_sz;
  } else if (*compiled_subst_sz < needed_sz) {
    void *tmp = xrealloc(*compiled_subst, needed_sz);
    *compiled_subst = tmp;
    *compiled_subst_sz = needed_sz;
  }
  memset(*compiled_subst, 0, *compiled_subst_sz);

  p = (char *) subst_pattern;
  while (*p) {
    if (*p == '\\') {
      is_escaped ^= 1;
      p++;
    }
    if (! is_escaped && *p == '$') {
      int good_variable = 1;
      int variable_num;
      int has_braces = (*(p+1) == '{');

      if (has_braces) {
        char closing_brace;
        good_variable = sscanf(p+1, "{%u%c", &variable_num, &closing_brace);
        if (closing_brace != '}')
          good_variable = 0;
      } else {
        good_variable = sscanf(p+1, "%u", &variable_num);
      }
      if (good_variable) {
        if ((*compiled_subst)[n_elems].elem_type == resubst_literal) {
          /* Terminate the previous literal. */
          (*compiled_subst)[n_elems].elem_len =
              p - (*compiled_subst)[n_elems].begin;
        }
        if ((*compiled_subst)[n_elems].elem_type)
          n_elems++;
        (*compiled_subst)[n_elems].elem_type = resubst_variable;
        (*compiled_subst)[n_elems].variable_num = variable_num;

        /* Scan past '$', braces, and variable number. */
        p++;
        if (has_braces)
          p++;
        while (isdigit(*p))
          p++;
        if (has_braces)
          p++;

      } else {
        /* Start a new literal if necessary. */
        if (! (*compiled_subst)[n_elems].elem_type) {
          (*compiled_subst)[n_elems].elem_type = resubst_literal;
          (*compiled_subst)[n_elems].begin = p;
        }
        p++;
      }
    } else {
      /* TODO(jhinds): Expand escape sequences as Perl would do.  For literals
       * (e.g. '\$' or '\\') it already works, but for metachars like '\t', it
       * does not. */
      if ((*compiled_subst)[n_elems].elem_type != resubst_literal) {
        /* Current is uninitialized or variable element.  Begin a new
         * literal. */
        if ((*compiled_subst)[n_elems].elem_type)
          n_elems++;
        (*compiled_subst)[n_elems].elem_type = resubst_literal;
        (*compiled_subst)[n_elems].begin = p;
      }
      is_escaped = 0;
      p++;
    }
  }
  if (! (*compiled_subst)[n_elems].elem_type) {
    (*compiled_subst)[n_elems].elem_type = resubst_literal;
    (*compiled_subst)[n_elems].begin = p;
  }
  if ((*compiled_subst)[n_elems].elem_type == resubst_literal) {
    (*compiled_subst)[n_elems].elem_len =
        p - (*compiled_subst)[n_elems].begin;
  }
  return n_elems + 1;
}


static char * _crush_re_expand_subst(const char *subject,
                                     const char *substitution,
                                     int *ovector, int n_pairs,
                                     struct crush_resubst_elem *compiled_subst,
                                     size_t n_subst_elems,
                                     char **target, size_t *target_sz,
                                     size_t *target_offset) {
  unsigned int capt_var;
  int i;

  assert(*target);
  assert(*target_sz > 0);

  for (i = 0; i < n_subst_elems; i++) {
    if (compiled_subst[i].elem_type == resubst_variable) {
      int capt_var = compiled_subst[i].variable_num;
      for (;;) {
        int var_val_len = pcre_copy_substring(subject, ovector, n_pairs,
                                              capt_var,
                                              *target + *target_offset,
                                              *target_sz - *target_offset);
        if (var_val_len == PCRE_ERROR_NOMEMORY) {
          int add_size = ovector[capt_var*2 + 1] - ovector[capt_var*2] + 32;
          char *tmp = xrealloc(*target, *target_sz + add_size);
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
      int len = compiled_subst[i].elem_len;
      if (*target_offset + len >= *target_sz) {
        int resize = *target_offset + len + 32;
        char *tmp = xrealloc(*target, resize);
        *target = tmp;
        *target_sz = resize;
      }
      strncat(*target, compiled_subst[i].begin, compiled_subst[i].elem_len);
      *target_offset += compiled_subst[i].elem_len;
    }
    (*target)[*target_offset] = '\0';
  }
  return *target;
}

char * crush_re_substitute(pcre *re, pcre_extra *re_extra,
                           struct crush_resubst_elem *compiled_subst,
                           size_t n_subst_elems,
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
    *target = xmalloc(*target_sz);
  }
  if (*target_sz < subject_len) {
    char *tmp = xrealloc(*target, subject_len);
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
        char *tmp = xrealloc(*target, *target_sz + add_size);
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
                                 compiled_subst, n_subst_elems, 
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

#endif /* HAVE_LIBPCRE && HAVE_PCRE_H */

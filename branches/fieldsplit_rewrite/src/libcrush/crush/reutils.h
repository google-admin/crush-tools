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

/** @file reutils.h
  * @brief Regular expression utilities.
  *
  *
  * Example:

#include <crush/reutils.h>

int main (int argc, char *argv[]) {
  char *str = argv[1];
  char *pattern = argv[2];
  char *subst = argv[3];

  char *subst_buffer = NULL;
  size_t subst_buffer_sz = 0;
  pcre *re;
  pcre_extra *re_extra;
  int re_flags = 0;
  int subst_globally = 0;
  const char *re_error;
  int re_err_offset;

  if (argc > 4)
    re_flags = crush_re_make_flags(argv[4], &subst_globally);

  re = pcre_compile(pattern, re_flags, &re_error, &re_err_offset, NULL);
  if (! re) {
    fprintf(stderr, "re compile failed: %s\n", re_error);
    exit(1);
  }

  re_extra = pcre_study(re, 0, &re_error);

  if (crush_re_substitute(re, re_extra, str, subst,
                          &subst_buffer, &subst_buffer_sz, subst_globally)) {
    printf("%s\n", subst_buffer);
  } else {
    fprintf(stderr, "substitution failed.\n");
    exit(1);
  }
  return 0;
}

 */

#ifdef HAVE_CONFIG_H
#include <crush/config.h>
#endif

#ifdef HAVE_PCRE_H
#include <pcre.h>

#ifndef REUTILS_H
#define REUTILS_H
/** @brief translates Perl-style RE modifiers into PCRE flags.
  *
  * Global substitution is not a PCRE flag, so it is stored in a separate
  * variable.
  *
  * @arg modifiers Perl RE modifier string.
  * @arg global value is set to 1 if 'g' is one of the modifiers.
  *
  * @return flags which can be passed to pcre_compile().
  */
int crush_re_make_flags(const char const *modifiers, int *global);

/** @brief performs a regex substitution on a subject string.
  *
  * @arg re a compile match regular expression
  * @arg re_extra result of pcre_study() against re.
  * @arg subject the string to match against.
  * @arg substitution the substitution pattern string.
  * @arg target pointer to a dynamically-allocated buffer to hold the result of
  *             the substitution.
  * @arg target_sz pointer to the size of *target.
  * @arg subst_globally non-zero to perform global substitution.
  *
  * @return the target string, or NULL on error.
  */
char * crush_re_substitute(pcre *re, pcre_extra *re_extra,
                           const char *subject,
                           const char *substitution,
                           char **target, size_t *target_sz,
                           int subst_globally);

#endif /* REUTILS_H */
#endif /* HAVE_PCRE_H */

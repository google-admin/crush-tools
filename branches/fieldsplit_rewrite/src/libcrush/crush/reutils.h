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
  * From a high level, this interface is used by doing the following.
  *  - (optional) translate Perl re modifiers to PCRE flags with
  *    crush_re_make_flags().
  *  - compile a PCRE matching regex with pcre_compile().
  *  - compile the substitution pattern using crush_resubst_compile().
  *  - perform substitutions using crush_re_substitute().
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

  struct crush_resubst_elem *compiled_subst = NULL;
  size_t compiled_subst_sz = 0;
  int n_subst_elems = 0;

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

  n_subst_elems = crush_resubst_compile(subst, &compiled_subst,
                                        &compiled_subst_sz);

  if (crush_re_substitute(re, re_extra,
                          compiled_subst, n_subst_elems,
                          str, subst,
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

#ifndef REUTILS_H
#define REUTILS_H

#ifdef HAVE_PCRE_H
#include <pcre.h>

/** @brief describes a part of a regex substitution.
  * Substitution strings are compiled down to a list of elements which are
  * either string literals or captured variable references.
  */
struct crush_resubst_elem {
  /** @brief what kind of thing this is. */
  enum { resubst_literal = 1,
         resubst_variable = 2 } elem_type;
  /** @brief for literals, a pointer into the substitution string where it
    * begins.  For variables, the captured number number. */
  union { char *begin;
          int variable_num; };
  /** @brief for literals only, how long the substring is. */
  size_t elem_len;
};

/** @brief compiles a substitution pattern.
  *
  * compiled_subst and compiled_subst_sz will be modified if they are NULL
  * or not large enough.
  *
  * @arg subst_pattern the string to substitute into a regex match.
  * @arg compiled_subst where the output should be stored.
  * @arg compiled_subst_sz how much memory is allocated to compiled_subst.
  */
int crush_resubst_compile(const char *subst_pattern,
                          struct crush_resubst_elem **compiled_subst,
                          size_t *compiled_subst_sz);

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
  * @arg compiled_subst result of crush_resubst_compile().
  * @arg n_subst_elems the number of elements in compiled_subst.
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
                           struct crush_resubst_elem *compiled_subst,
                           size_t n_subst_elems,
                           const char *subject,
                           const char *substitution,
                           char **target, size_t *target_sz,
                           int subst_globally);

#endif /* HAVE_PCRE_H */
#endif /* REUTILS_H */

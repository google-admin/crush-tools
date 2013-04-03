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

#include <stdio.h>
#include <string.h>
#include <crush/reutils.h>
#include "unittest.h"

#if HAVE_PCRE_H && HAVE_LIBPCRE

#define INIT_TEST(regex) \
  pcre *re; \
  const char *re_error; \
  int re_err_offset; \
  struct crush_resubst_elem *compiled_subst = NULL; \
  size_t compiled_subst_sz = 0; \
  int n_subst_elems = 0; \
  char *target = NULL; \
  size_t target_sz = 0; \
  re = pcre_compile((regex), 0, &re_error, &re_err_offset, NULL);


#define TEST(subtest, subject, substitution, global, expected) \
  do { \
    char *substitute_retval; \
    n_subst_elems = crush_resubst_compile((substitution), \
                                          &compiled_subst, \
                                          &compiled_subst_sz); \
    substitute_retval = crush_re_substitute( \
        re, NULL, compiled_subst, n_subst_elems, (subject), \
        (substitution), &target, &target_sz, (global)); \
    ASSERT_TRUE(substitute_retval != NULL, \
                "crush_re_substitute: return non-null"); \
    ASSERT_STR_EQ((expected), target, TEST_DESC); \
  } while (0)


#define FINISH_TEST() \
  if (target) \
    free(target);


void test_make_flags() {
  int flags, is_global = 0;

  flags = crush_re_make_flags("", &is_global);
  ASSERT_INT_EQ(0, flags, "crush_re_make_flags: translate empty flag string");
  ASSERT_INT_EQ(0, is_global, "crush_re_make_flags: empty flags not global");

  flags = crush_re_make_flags("i", &is_global);
  ASSERT_INT_EQ(PCRE_CASELESS, flags, "crush_re_make_flags: caseless flag");
  ASSERT_INT_EQ(0, is_global, "crush_re_make_flags: caseless flags not global");

  flags = crush_re_make_flags("ig", &is_global);
  ASSERT_INT_EQ(PCRE_CASELESS, flags,
                "crush_re_make_flags: caseless global flag");
  ASSERT_INT_EQ(1, is_global, "crush_re_make_flags: set global flag");
}

void test_resubst_compile() {
  struct crush_resubst_elem *subst_elems = NULL;
  size_t subst_elems_sz = 0;
  int n_elems;
  char pattern[64];

  strcpy(pattern, "");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  ASSERT_INT_EQ(1, n_elems, "crush_resubst_compile: empty pattern retval");
  ASSERT_INT_EQ(resubst_literal, subst_elems[0].elem_type,
                "crush_resubst_compile: literal element type");
  ASSERT_PTR_EQ(pattern, subst_elems[0].begin,
                "crush_resubst_compile: start of element");
  ASSERT_LONG_EQ(0, subst_elems[0].elem_len,
                 "crush_resubst_compile: element length");

  strcpy(pattern, "$1");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  ASSERT_INT_EQ(1, n_elems, "crush_resubst_compile: number of elements");
  ASSERT_INT_EQ(resubst_variable, subst_elems[0].elem_type,
                "crush_resubst_compile: variable element type");
  ASSERT_INT_EQ(1, subst_elems[0].variable_num,
                "crush_resubst_compile: variable number");

  strcpy(pattern, "hello$1world");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  ASSERT_INT_EQ(3, n_elems, "crush_resubst_compile: number of elements");
  ASSERT_INT_EQ(resubst_literal, subst_elems[0].elem_type,
                "crush_resubst_compile: first element type");
  ASSERT_PTR_EQ(pattern, subst_elems[0].begin,
                "crush_resubst_compile: start of first element");
  ASSERT_LONG_EQ(5, subst_elems[0].elem_len,
                 "crush_resubst_compile: first element length");

  ASSERT_INT_EQ(resubst_variable, subst_elems[1].elem_type,
                "crush_resubst_compile: second element type");
  ASSERT_INT_EQ(1, subst_elems[1].variable_num,
                "crush_resubst_compile: second variable number");

  ASSERT_INT_EQ(resubst_literal, subst_elems[2].elem_type,
                "crush_resubst_compile: third element type");
  ASSERT_PTR_EQ(pattern + 7, subst_elems[2].begin,
                "crush_resubst_compile: start of third element");
  ASSERT_LONG_EQ(5, subst_elems[2].elem_len,
                 "crush_resubst_compile: third element length");

  strcpy(pattern, "${1${}$hello");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  ASSERT_INT_EQ(1, n_elems, "crush_resubst_compile: number of elements");
  ASSERT_INT_EQ(resubst_literal, subst_elems[0].elem_type,
                "crush_resubst_compile: element type");
  ASSERT_PTR_EQ(pattern, subst_elems[0].begin,
                "crush_resubst_compile: start of element");
  ASSERT_LONG_EQ(strlen(pattern), subst_elems[0].elem_len,
                 "crush_resubst_compile: element length");
}

void test_basic_substitution() {
#define TEST_DESC "basic substitution"
  INIT_TEST("l");
  TEST(1, "hello", "r", 0, "herlo");
  TEST(2, "hello", "\\$1", 0, "he$1lo");
  TEST(3, "hello", "\\\\", 0, "he\\lo");
  FINISH_TEST();
#undef TEST_DESC
}

void test_global_substitution() {
#define TEST_DESC "global substitution"
  INIT_TEST("l");
  TEST(1, "hello", "r", 1, "herro");
  TEST(2, "hello", "", 1, "heo");
  FINISH_TEST();
#undef TEST_DESC
}

void test_good_variables() {
#define TEST_DESC "variable substitution"
  INIT_TEST("(l+)");
  TEST(1, "hello", "$1", 0, "hello");
  TEST(2, "hello", "${1}", 0, "hello");
  TEST(3, "hello", "$1$2", 0, "hello");
  TEST(4, "hello", "$1$1", 0, "hellllo");
  FINISH_TEST();
#undef TEST_DESC
}

void test_multi_variables() {
#define TEST_DESC "multiple variable substitution"
  INIT_TEST("^(.)(.*)");
  TEST(1, "hello", "$2$1ay", 0, "ellohay");
  TEST(2, "hello", "$2$1ay", 1, "ellohay");
  FINISH_TEST();
#undef TEST_DESC
}

void test_bad_variables() {
#define TEST_DESC "malformed variable substitution"
  INIT_TEST("(l+)");
  TEST(1, "hello", "${}", 0, "he${}o");
  TEST(2, "hello", "$x", 0, "he$xo");
  TEST(3, "hello", "${1", 0, "he${1o");
  FINISH_TEST();
#undef TEST_DESC
}

int main (int argc, char *argv[]) {
  test_make_flags();
  test_resubst_compile();
  test_basic_substitution();
  test_global_substitution();
  test_good_variables();
  test_multi_variables();
  test_bad_variables();
  return unittest_has_error;
}

# else /* not HAVE_PCRE_H && HAVE_LIBPCRE */
int main (int argc, char *argv[]) {
  printf("SKIP: pcre is not installed.\n");
  exit(0);
}
#endif

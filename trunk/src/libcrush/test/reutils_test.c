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
#include <crush/reutils.h>

#define INIT_TEST(description, regex) \
  char *desc = (description); \
  pcre *re; \
  const char *re_error; \
  int re_err_offset; \
  struct crush_resubst_elem *compiled_subst = NULL; \
  size_t compiled_subst_sz = 0; \
  int n_subst_elems = 0; \
  char *target = NULL; \
  size_t target_sz = 0; \
  int has_error = 0; \
  re = pcre_compile((regex), 0, &re_error, &re_err_offset, NULL);


#define TEST(subtest, subject, substitution, global, expected) \
  n_subst_elems = crush_resubst_compile((substitution), \
                                        &compiled_subst, \
                                        &compiled_subst_sz); \
  if (crush_re_substitute(re, NULL, compiled_subst, n_subst_elems, \
                          (subject), (substitution), \
                          &target, &target_sz, (global))) { \
    if (strcmp(target, (expected)) != 0) { \
      printf("FAIL: reutils: %s %d: expected \"%s\", got \"%s\".\n", \
             desc, subtest, (expected), target); \
      has_error = 1; \
    } \
  } else { \
    printf("FAIL: reutils: %s %d: error performing substitution.\n", \
           desc, subtest); \
    has_error = 1; \
  }


#define FINISH_TEST() \
  if (target) \
    free(target); \
  if (! has_error) \
    printf("PASS: reutils: %s.\n", desc); \
  return has_error;


int test_make_flags() {
  char *desc = "make flags";
  char *modifiers;
  int flags, is_global = 0;
  int has_error = 0;

  flags = crush_re_make_flags("", &is_global);
  if (flags != 0 || is_global != 0) {
    printf("FAIL: reutils: %s %d: error translating flags.\n", \
           desc, 1); \
    has_error = 1;
  }

  flags = crush_re_make_flags("i", &is_global);
  if (flags != PCRE_CASELESS || is_global != 0) {
    printf("FAIL: reutils: %s %d: error translating flags.\n", \
           desc, 2); \
    has_error = 1;
  }

  flags = crush_re_make_flags("ig", &is_global);
  if (flags != PCRE_CASELESS || ! is_global) {
    printf("FAIL: reutils: %s %d: error translating flags.\n", \
           desc, 3); \
    has_error = 1;
  }
  if (! has_error)
    printf("PASS: reutils: %s.\n", desc);
  return has_error;
}

int test_resubst_compile() {
  char *desc = "resubst compile";
  struct crush_resubst_elem *subst_elems = NULL;
  size_t subst_elems_sz = 0;
  int n_elems, has_error = 0;
  char pattern[64];

  strcpy(pattern, "");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  if (n_elems != 1) {
    printf("FAIL: reutils: %s %d: compiled %d elems instead of %d.\n",
           desc, 1, n_elems, 1);
    has_error = 1;
  } else {
    if (subst_elems[0].elem_type != resubst_literal) {
      printf("FAIL: reutils: %s %d: bad element type.\n", desc, 1);
      has_error = 1;
    }
    if (subst_elems[0].begin != pattern) {
      printf("FAIL: reutils: %s %d: bad start of element.\n", desc, 1);
      has_error = 1;
    }
    if (subst_elems[0].elem_len != 0) {
      printf("FAIL: reutils: %s %d: bad length of element.\n", desc, 1);
      has_error = 1;
    }
  }

  strcpy(pattern, "$1");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  if (n_elems != 1) {
    printf("FAIL: reutils: %s %d: compiled %d elems instead of %d.\n",
           desc, 2, n_elems, 1);
    has_error = 1;
  } else {
    if (subst_elems[0].elem_type != resubst_variable) {
      printf("FAIL: reutils: %s %d: bad element type.\n", desc, 2);
      has_error = 1;
    }
    if (subst_elems[0].variable_num != 1) {
      printf("FAIL: reutils: %s %d: bad variable number.\n", desc, 2);
      has_error = 1;
    }
  }

  strcpy(pattern, "hello$1world");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  if (n_elems != 3) {
    printf("FAIL: reutils: %s %d: compiled %d elems instead of %d.\n",
           desc, 3, n_elems, 3);
    has_error = 1;
  } else {
    if (subst_elems[0].elem_type != resubst_literal) {
      printf("FAIL: reutils: %s %d: bad element 1 type.\n", desc, 3);
      has_error = 1;
    } else if (subst_elems[0].begin != pattern) {
      printf("FAIL: reutils: %s %d: bad start of element 1.\n", desc, 3);
      has_error = 1;
    } else if (subst_elems[0].elem_len != 5) {
      printf("FAIL: reutils: %s %d: bad length of element 1.\n", desc, 3);
      has_error = 1;
    }

    if (subst_elems[1].elem_type != resubst_variable) {
      printf("FAIL: reutils: %s %d: bad element 2 type.\n", desc, 3);
      has_error = 1;
    } else if (subst_elems[1].variable_num != 1) {
      printf("FAIL: reutils: %s %d: bad variable number.\n", desc, 3);
      has_error = 1;
    }

    if (subst_elems[2].elem_type != resubst_literal) {
      printf("FAIL: reutils: %s %d: bad element 3 type.\n", desc, 3);
      has_error = 1;
    } else if (subst_elems[2].begin != pattern + 7) {
      printf("FAIL: reutils: %s %d: bad start of element 3.\n", desc, 3);
      has_error = 1;
    } else if (subst_elems[2].elem_len != 5) {
      printf("FAIL: reutils: %s %d: bad length of element 3.\n", desc, 3);
      has_error = 1;
    }
  }

  strcpy(pattern, "${1${}$hello");
  n_elems = crush_resubst_compile(pattern, &subst_elems, &subst_elems_sz);
  if (n_elems != 1) {
    printf("FAIL: reutils: %s %d: compiled %d elems instead of %d.\n",
           desc, 4, n_elems, 1);
    has_error = 1;
  } else {
    if (subst_elems[0].elem_type != resubst_literal) {
      printf("FAIL: reutils: %s %d: bad element type.\n", desc, 4);
      has_error = 1;
    } else {
      if (subst_elems[0].begin != pattern) {
        printf("FAIL: reutils: %s %d: bad start of element.\n", desc, 4);
        has_error = 1;
      }
      if (subst_elems[0].elem_len != strlen(pattern)) {
        printf("FAIL: reutils: %s %d: bad element length.\n", desc, 4);
        has_error = 1;
      }
    }
  }

  if (! has_error)
    printf("PASS: reutils: %s.\n", desc);
  return has_error;
}

int test_basic_substitution() {
  INIT_TEST("basic substitution", "l")
  TEST(1, "hello", "r", 0, "herlo")
  TEST(2, "hello", "\\$1", 0, "he$1lo")
  TEST(3, "hello", "\\\\", 0, "he\\lo")
  FINISH_TEST()
}

int test_global_substitution() {
  INIT_TEST("global substitution", "l")
  TEST(1, "hello", "r", 1, "herro")
  TEST(2, "hello", "", 1, "heo")
  FINISH_TEST()
}

int test_good_variables() {
  INIT_TEST("variable substitution", "(l+)")
  TEST(1, "hello", "$1", 0, "hello")
  TEST(2, "hello", "${1}", 0, "hello")
  TEST(3, "hello", "$1$2", 0, "hello")
  TEST(4, "hello", "$1$1", 0, "hellllo")
  FINISH_TEST()
}

int test_multi_variables() {
  INIT_TEST("multiple variable substitution", "^(.)(.*)")
  TEST(1, "hello", "$2$1ay", 0, "ellohay")
  TEST(2, "hello", "$2$1ay", 1, "ellohay")
  FINISH_TEST()
}

int test_bad_variables() {
  INIT_TEST("malformed variable substitution", "(l+)")
  TEST(1, "hello", "${}", 0, "he${}o")
  TEST(2, "hello", "$x", 0, "he$xo")
  TEST(3, "hello", "${1", 0, "he${1o")
  FINISH_TEST()
}

int main (int argc, char *argv[]) {
  int n_failures = 0;
  n_failures += test_make_flags();
  n_failures += test_resubst_compile();
  n_failures += test_basic_substitution();
  n_failures += test_global_substitution();
  n_failures += test_good_variables();
  n_failures += test_multi_variables();
  n_failures += test_bad_variables();
  if (n_failures)
    exit(1);
  exit(0);
}

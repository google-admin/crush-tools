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
  char *target = NULL; \
  size_t target_sz = 0; \
  int has_error = 0; \
  re = pcre_compile((regex), 0, &re_error, &re_err_offset, NULL);


#define TEST(subtest, subject, substitution, global, expected) \
  if (crush_re_substitute(re, NULL, (subject), (substitution), \
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
  TEST(1, "hello", "${1", 0, "he${1o")
  FINISH_TEST()
}

int main (int argc, char *argv[]) {
  int n_failures = 0;
  n_failures += test_make_flags();
  n_failures += test_basic_substitution();
  n_failures += test_global_substitution();
  n_failures += test_good_variables();
  n_failures += test_multi_variables();
  n_failures += test_bad_variables();
  if (n_failures)
    exit(1);
  exit(0);
}

/*****************************************
   Copyright 2008 Google Inc.

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
#include <stdlib.h>
#include <crush/qsort_helper.h>
#include "unittest.h"


void test_qsort_strcmp() {
  char *array[] = { "hello", "world", "there" };
  char *expected[] = { "hello", "there", "world" };
  qsort(array, 3, sizeof(char *), (qsort_cmp_func_t) qsort_strcmp);
  ASSERT_STR_ARRAY_EQ(expected, array, 3, "qsort_strcmp: valid sort order");
}

void test_qsort_intcmp() {
  int array[] = { 3, 5, -1, 2, 1 };
  int expected[] = { -1, 1, 2, 3, 5 };
  qsort(array, 5, sizeof(int), (qsort_cmp_func_t) qsort_intcmp);
  ASSERT_INT_ARRAY_EQ(expected, array, 5, "qsort_intcmp: valid sort order");
}

void test_qsort_uintcmp() {
  unsigned int array[] = { 3, 5, 1, 2, 1 };
  unsigned int expected[] = { 1, 1, 2, 3, 5 };
  qsort(array, 5, sizeof(int), (qsort_cmp_func_t) qsort_uintcmp);
  ASSERT_INT_ARRAY_EQ(expected, array, 5, "qsort_uintcmp: valid sort order");
}

int main(int argc, char *argv[]) {
  test_qsort_strcmp();
  test_qsort_intcmp();
  test_qsort_uintcmp();
  return unittest_has_error;
}


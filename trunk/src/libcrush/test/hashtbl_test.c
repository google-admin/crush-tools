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
#include <stdlib.h>
#include <string.h>
#include <crush/hashtbl.h>

int has_failures = 0;

#define test_assert(cond, msg) \
  do { \
    if (! (cond)) { \
      fprintf(stderr, "FAIL: %s\n", (msg)); \
      has_failures = 1; \
    } else { \
      fprintf(stderr, "PASS: %s\n", (msg)); \
    } \
  } while (0)


int n_calls = 0;
void call_for_each_test(void *data) {
  n_calls++;
}


int main(int argc, char *argv[]) {
  hashtbl_t ht;
  char *data;
  char **keys;
  int i, j;
  test_assert(ht_init(&ht, 5, NULL, free) == 0, "clean initialization");

  for (j = 0; j < 3; j++) {
    for (i = 0; i < 10; i++) {
      char key[12];

      sprintf(key, "hello %d", i);
      data = strdup("world");

      ht_put(&ht, key, data);
      test_assert(ht_get(&ht, key) == data, "look up newly-put key");
    }
  }
  test_assert(ht.nelems == 10, "ht_put updats elements correctly");

  test_assert(ht_get(&ht, "hello 13") == NULL, "look up non-existing key");
  ht_call_for_each(&ht, call_for_each_test);
  test_assert(n_calls == 10, "ht_call_for_each");

  keys = malloc(ht.nelems * sizeof(char *));
  test_assert(ht_keys(&ht, keys) == 10, "ht_keys");
/*
  for (i = 0; i < 10; i++) {
    puts(keys[i]);
  }
*/

  return has_failures;
}

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
#include "unittest.h"

int has_failures = 0;

int n_calls = 0;
void call_for_each_test(void *data) {
  n_calls++;
}

void call_for_each2_test(void *node_data, int *aux_data) {
  (*aux_data)++;
}


int main(int argc, char *argv[]) {
  hashtbl_t ht;
  char **keys;
  int i, j, retval;
  retval = ht_init(&ht, 5, NULL, free);
  ASSERT_INT_EQ(0, retval, "ht_init: clean initialization");

  for (j = 0; j < 3; j++) {
    for (i = 0; i < 10; i++) {
      char *data;
      char key[12];

      sprintf(key, "hello %d", i);
      data = strdup("world");  /* free()d by ht_destroy(). */

      retval = ht_put(&ht, key, data);
      ASSERT_INT_EQ(0, retval, "ht_put: returns 0 on success.");
      ASSERT_TRUE(ht_get(&ht, key) == data, "ht_get: look up newly-put key");
    }
  }
  ASSERT_LONG_EQ(10L, ht.nelems, "ht_put: update element count correctly");

  ASSERT_TRUE(ht_get(&ht, "hello 13") == NULL,
              "ht_get: look up non-existing key");
  ht_call_for_each(&ht, call_for_each_test);
  ASSERT_INT_EQ(10, n_calls, "ht_call_for_each: verify repeated invokations");

  i = 0;
  ht_call_for_each2(&ht, call_for_each2_test, &i);
  ASSERT_INT_EQ(10, i, "ht_call_for_each2: verify repeated invokations");

  keys = malloc(ht.nelems * sizeof(char *));
  retval = ht_keys(&ht, keys);
  ASSERT_INT_EQ(10, retval, "ht_keys: returns the number of keys.");
  /* TODO(jhinds): verify the contents of keys[] */

  ht_delete(&ht, "hello 0");
  ASSERT_TRUE(ht_get(&ht, "hello 0") == NULL,
              "ht_delete: removes entry successfully");
  ASSERT_LONG_EQ(9L, ht.nelems, "ht_delete: updates element count correctly");

  return unittest_has_error;
}

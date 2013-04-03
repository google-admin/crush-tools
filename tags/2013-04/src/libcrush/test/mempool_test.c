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
#include <string.h>
#include <crush/mempool.h>
#include "unittest.h"


int main(int argc, char *argv[]) {
  int test_int = 0xffffffff;
  void *ptr_a, *ptr_b;
  mempool_t *pool = NULL;
  pool = mempool_create(16);

  ASSERT_TRUE(pool != NULL, "mempool_create returns valid pointer");
  ASSERT_LONG_EQ(16, pool->page_size, "mempool_create sets page size");
  ASSERT_LONG_EQ(1, pool->n_pages, "mempool_create initializes one page");
  ASSERT_TRUE(pool->pages != NULL, "mempool_create page list not null");
  ASSERT_TRUE(pool->pages[0].buffer != NULL, "mempool_create allocates page");
  ASSERT_LONG_EQ(0, pool->pages[0].next,
                 "mempool_create initializes page->next");

  ptr_a = mempool_alloc(pool, sizeof(test_int));
  ASSERT_TRUE(ptr_a != NULL, "mempool_alloc returns valid pointer");
  ASSERT_LONG_EQ(sizeof(test_int), pool->pages[0].next,
                 "mempool_alloc sets next location correctly");

  *((int *) ptr_a) = test_int;
  ptr_b = mempool_add(pool, "hello world", strlen("hello world") + 1);
  ASSERT_TRUE(ptr_b != NULL, "mempool_add returns valid pointer");
  ASSERT_STR_EQ("hello world", (char *)ptr_b,
                "mempool_add copies data into pool");
  ASSERT_INT_EQ(test_int, *((int *) ptr_a),
                "mempool_add doesn't clobber pool data");
  ASSERT_TRUE(ptr_b == ptr_a + sizeof(test_int),
              "mempool_add puts new data in correct place");
  ASSERT_LONG_EQ(1, pool->next_unfull,
                 "mempool_alloc detects full buffer");
  ASSERT_LONG_EQ(1, pool->n_pages,
                 "mempool_alloc doesn't allocate new pages needlessly");
  mempool_add(pool, "goodbye world", strlen("goodbye world") + 1);
  ASSERT_LONG_EQ(2, pool->n_pages,
                 "mempool_alloc adds new pages as necessary");
  mempool_destroy(pool);

  return 0;
}

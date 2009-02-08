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

#include <crush/mempool.h>
#include <stdio.h>
#include <string.h>

#define test_assert(cond, msg) \
  do { \
    if (! (cond)) \
      fprintf(stderr, "FAIL: %s\n", (msg)); \
    else \
      fprintf(stderr, "PASS: %s\n", (msg)); \
  } while (0)

int main(int argc, char *argv[]) {
  int test_int = 0xffffffff;
  void *ptr_a, *ptr_b;
  mempool_t *pool = NULL;
  pool = mempool_create(16);

  test_assert(pool != NULL, "mempool_create returns valid pointer");
  test_assert(pool->page_size == 16, "mempool_create sets page size");
  test_assert(pool->n_pages == 1, "mempool_create initializes one page");
  test_assert(pool->pages != NULL, "mempool_create page list not null");
  test_assert(pool->pages[0].buffer != NULL, "mempool_create allocates page");
  test_assert(pool->pages[0].next == 0,
              "mempool_create initializes page->next");

  ptr_a = mempool_alloc(pool, sizeof(test_int));
  test_assert(ptr_a != NULL, "mempool_alloc returns valid pointer");
  test_assert(pool->pages[0].next == sizeof(test_int),
              "mempool_alloc sets next location correctly");
  *((int *) ptr_a) = test_int;

  ptr_b = mempool_add(pool, "hello world", strlen("hello world") + 1);
  test_assert(ptr_b != NULL, "mempool_add returns valid pointer");
  test_assert(strcmp((char *)ptr_b, "hello world") == 0,
              "mempool_add copies data into pool");
  test_assert(*((int *) ptr_a) == test_int,
              "mempool_add doesn't clobber pool data");
  test_assert(ptr_b == ptr_a + sizeof(test_int),
              "mempool_add puts new data in correct place");
  test_assert(pool->next_unfull == 1,
              "mempool_alloc detects full buffer");
  test_assert(pool->n_pages == 1,
              "mempool_alloc doesn't allocate new pages needlessly");
  mempool_add(pool, "goodbye world", strlen("goodbye world") + 1);
  test_assert(pool->n_pages == 2,
              "mempool_alloc adds new pages as necessary");
  mempool_destroy(pool);

  return 0;
}

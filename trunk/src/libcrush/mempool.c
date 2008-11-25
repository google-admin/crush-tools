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

#include <mempool.h>
#include <string.h>             /* memcpy() */

/* returns the amount of memory remaining within a given page. */
#define bytes_available_in_page(pool, page_idx) \
  ((pool)->page_size - (pool)->pages[page_idx].next)

/* Add a page of memory to the pool. */
static void * _mempool_add_page(mempool_t *pool) {
  void *tmp;
  int i = pool->n_pages;
  if (! pool->pages) {
    tmp = malloc(sizeof(struct _mempool_page));
  } else {
    tmp = realloc(pool->pages,
                  sizeof(struct _mempool_page) * (pool->n_pages + 1));
  }
  if (! tmp)
    return NULL;
  pool->pages = tmp;
  pool->n_pages++;

  pool->pages[i].next = 0;
  pool->pages[i].buffer = malloc(pool->page_size);
  if (! pool->pages[i].buffer)
    return NULL;
  return pool->pages;
}

/* Allocate and initialize a mempool. */
mempool_t *mempool_create(size_t page_size) {
  mempool_t *pool;

  pool = malloc(sizeof(mempool_t));
  if (!pool) {
    return NULL;
  }
  memset(pool, 0, sizeof(mempool_t));
  pool->page_size = page_size;

  if (! _mempool_add_page(pool)) {
    free(pool);
    return NULL;
  }
  pool->next_unfull = 0;
  return pool;
}

/* Allocate memory in the pool and copy something into it. */
void * mempool_add(mempool_t * pool, const void *thing, size_t thing_size) {
  void *location;
  if (!pool || !thing || thing_size == 0)
    return NULL;
  location = mempool_alloc(pool, thing_size);
  if (location)
    memcpy(location, thing, thing_size);
  return location;
}

/* Reserve memory within the mempool */
void * mempool_alloc(mempool_t * pool, size_t n_bytes) {
  void *location = NULL;
  int i;
  if (!pool || n_bytes == 0 || n_bytes > pool->page_size)
    return NULL;

  /* find a page with enough room, or make a new page. */
  for (i = pool->next_unfull; i < pool->n_pages; i++) {
    if (bytes_available_in_page(pool, i) >= n_bytes)
      break;
  }

  if (i == pool->n_pages) {
    if (! _mempool_add_page(pool))
      return NULL;
  }

  location = pool->pages[i].buffer + pool->pages[i].next;
  pool->pages[i].next += n_bytes;

  /* reduce search time for subsequent allocations */
  for (i = pool->next_unfull;
       i < pool->n_pages && bytes_available_in_page(pool, i) == 0;
       i++) {
    pool->next_unfull++;
  }

  return location;
}

/* Free resources associated with a mempool. */
void mempool_destroy(mempool_t * pool) {
  int i;
  if (!pool)
    return;

  for (i = 0; i < pool->n_pages; i++) {
    if (pool->pages[i].buffer) {
      free(pool->pages[i].buffer);
    }
  }
  free(pool->pages);
  free(pool);
}

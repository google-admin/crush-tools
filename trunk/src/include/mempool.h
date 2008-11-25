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

/** @file mempool.h
  * @brief A simple memory pool API.  Since space occupied by things in the
  * pool cannot be reclaimed, this is mostly useful for situations where 
  * objects in the pool have the same lifetime as the pool itself.
  *
  * To avoid problems when allocating very large amounts of contiguous memory,
  * the pool consists of "pages" of memory blocks.  No item larger than
  * the page size can be stored in the pool.  But ideally the page size will be
  * enough to hold several items.  And optimally, the page size should be a
  * multiple of the size of the things to be stored.
  */
#include <stdlib.h>

#ifndef MEMPOOL_H
#define MEMPOOL_H

/** @brief for internal use only */
struct _mempool_page {
  size_t next; /* the next unreserved location in the buffer */
  void *buffer; /* a block of of memory */
};

/** @brief the memory pool data type.  Members of this struct should not be
 *  accessed by user code. */
typedef struct _mempool {
  size_t page_size;  /**< @brief the capacity of each "page" of memory. */
  size_t n_pages;    /**< @brief the number of pages currently allocated. */
  size_t next_unfull;/**< @brief the index of the first page with free space. */
  struct _mempool_page *pages; /**< @brief list of pages */
} mempool_t;

/** @brief tells how much memory is being used by a mempool's buffers.
  * Does not account for overhead. */
#define mempool_size( p ) \
	((p)->capacity * (p)->n_buffers)

/** @brief creates a new memory pool.
  * 
  * @param page_size how many bytes each buffer in the memory pool should be
  * able to hold.  This also represents the largest amount of memory which can
  * be allocated.
  * 
  * @return a newly-allocated memory pool
  */
mempool_t *mempool_create(size_t page_size);

/** @brief adds something to the memory pool.
  * 
  * @param pool the pool to which the thing should be added
  * @param thing the thing to add to the pool
  * @param thing_size the size of the thing to add
  * 
  * @return the address in the memory pool where the thing was stored, or NULL
  * if the thing is larger than the pool's page size, or if there was an error
  * allocating a new page.
  */
void *mempool_add(mempool_t * pool, const void *thing, size_t thing_size);

/** @brief reserves space within a memory pool
  * 
  * @param pool in which the space should be reserved
  * @param n_bytes the number of bytes to reserve
  * 
  * @return the address in the memory pool where the thing was stored,
  * or NULL if n_bytes was zero, if n_bytes is larger than the pool's
  * capacity, or allocating a new page failed.
  */
void *mempool_alloc(mempool_t * pool, size_t n_bytes);

/** @brief frees resources associated with a memory pool.
  * 
  * @param pool 
  */
void mempool_destroy(mempool_t * pool);

#endif /* MEMPOOL_H */

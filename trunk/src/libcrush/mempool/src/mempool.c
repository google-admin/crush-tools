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
#include <string.h>	/* memcpy() */

/** @brief creates a memory pool of the given size.
  * 
  * @param capacity how many bytes the memory pool should be able to hold
  * 
  * @return a newly-allocated memory pool
  */
mempool_t * mempool_create ( size_t capacity ) {
	mempool_t *pool;

	pool = malloc( sizeof ( mempool_t ) );
	if ( ! pool ) {
		return NULL;
	}

	pool->buffer = malloc( capacity );
	if ( ! pool->buffer ) {
		free(pool);
		return NULL;
	}

	pool->next = 0;
	pool->capacity = capacity;

	return pool;
}

/** @brief tells how much space is left in a memory pool.
  * 
  * @param pool the memory pool to be queried.
  * 
  * @return the number of unused bytes in the memory pool
  */
size_t mempool_available ( mempool_t *pool ) {
	if ( ! pool )
		return 0;

	return pool->capacity - pool->next;
}

/** @brief adds something to the memory pool.
  * 
  * @param pool the pool to which the thing should be added
  * @param thing the thing to add to the pool
  * @param thing_size the size of the thing to add
  * 
  * @return the address in the memory pool where the thing was stored,
  * or NULL if there wasn't enough room for the thing.
  */
void * mempool_add ( mempool_t *pool, const void *thing, size_t thing_size ) {
	void *location;

	if ( ! pool )
		return NULL;

	if ( ! thing || thing_size == 0 )
		return NULL;

	if ( mempool_available( pool ) < thing_size )
		return NULL;

	/* calculate the offset into the buffer where the thing should be
	   stored.
	 */
	location = pool->buffer + pool->next;

	if ( memcpy( location, thing, thing_size ) != location ) {
		return NULL;
	}

	pool->next += thing_size;

	return location;
}

/** @brief reserves space within a memory pool
  * 
  * @param pool in which the space should be reserved
  * @param n_bytes the number of bytes to reserve
  * 
  * @return the address in the memory pool where the thing was stored,
  * or NULL if there wasn't enough room.
  */
void * mempool_alloc ( mempool_t *pool, size_t n_bytes ) {
	void *location;

	if ( ! pool )
		return NULL;
	if ( ! n_bytes || n_bytes > pool->capacity - pool->next )
		return NULL;

	location = pool->buffer + pool->next;

	pool->next += n_bytes;
	return location;
}

/** @brief zeroes out pool usage.
  * 
  * @param pool the pool to be cleared.
  */
void mempool_reset ( mempool_t *pool ) {
	if ( ! pool )
		return;
	/* waste of time to zero out the buffer */
	pool->next = 0;
}

/** @brief frees resources associated with a memory pool.
  * 
  * @param pool 
  */
void mempool_destroy ( mempool_t *pool ) {
	if ( ! pool )
		return;

	if ( pool->buffer )
		free(pool->buffer);

	free(pool);
}


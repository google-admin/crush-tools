#include <stdlib.h>

#ifndef MEMPOOL_H
#define MEMPOOL_H

typedef struct _mempool
{
	size_t capacity;
	size_t next;
	void  *buffer;
} mempool_t;

#define mempool_capacity( p ) \
	(p)->capacity



/** @brief creates a memory pool of the given size.
  * 
  * @param capacity how many bytes the memory pool should be able to hold
  * 
  * @return a newly-allocated memory pool
  */
mempool_t * mempool_create ( size_t capacity );

/** @brief tells how much space is left in a memory pool.
  * 
  * @param pool the memory pool to be queried.
  * 
  * @return the number of unused bytes in the memory pool
  */
size_t mempool_available ( mempool_t *pool );

/** @brief adds something to the memory pool.
  * 
  * @param pool the pool to which the thing should be added
  * @param thing the thing to add to the pool
  * @param thing_size the size of the thing to add
  * 
  * @return the address in the memory pool where the thing was stored,
  * or NULL if there wasn't enough room for the thing.
  */
void * mempool_add ( mempool_t *pool, const void *thing, size_t thing_size );

/** @brief zeroes out pool usage.
  * although they may not be immediately cleared out, existing references
  * to pool content should be considered invalid after a call to
  * mempool_reset().  calls to mempool_add() will overwrite existing
  * content.
  * 
  * @param pool the pool to be cleared.
  */
void mempool_reset ( mempool_t *pool );

/** @brief frees resources associated with a memory pool.
  * 
  * @param pool 
  */
void mempool_destroy ( mempool_t *pool );

#endif /* MEMPOOL_H */


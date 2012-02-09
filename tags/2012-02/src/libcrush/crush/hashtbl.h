/*****************************************
   Copyright 2008, 2009 Google Inc.

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

/** @file hashtbl.h
  * @brief Interface for the hashtbl library.
  *
  * This is a chained hashtable implementation, internally utilizing the
  * bstree library.  Note that for all key comparisons (in ht_put(), ht_get(),
  * ht_delete(), etc.) strcmp() is used for comparing the keys.  If support for
  * keys containing null bytes is a requirement, use hashtbl2 instead.
  *
  * Because of the use of a mempool for key allocation, there is a
  * 4095-character limit on key length.
  */

#include <stdlib.h>
#include <string.h>  /* strcmp(), strlen() */
#include <crush/bstree.h>
#include <crush/hashfuncs.h>
#include <crush/mempool.h>


#ifndef HASHTBL_H
#define HASHTBL_H

/** @brief the hashtable data type. */
typedef struct _hashtbl {
  size_t nelems;  /**< number of elements in the hashtable */
  size_t arrsz;   /**< size of the following array */
  bstree_t **arr;  /**< array of binary trees */
  /** hash function to use - see hashfuncs.h */
  unsigned int (*hash) (unsigned char *);
  /** memory-freeing function to call against an entry's data */
  void (*free) (void *);
  mempool_t *ht_elem_pool; /**< a pool for allocating elements */
  mempool_t *key_pool;     /**< a pool for allocating key strings */
} hashtbl_t;

/** @brief a key/value pair within the hashtable */
typedef struct _ht_elem {
  char *key;  /**< string lookup key for the element */
  void *data; /**< data to store in this element */
} ht_elem_t;

/** @brief initializes a new hashtable.  the memfree function should be
  * specified iff the payload of a node will need to be deallocated when
  * the hashtable is destroyed.  if a NULL hash function is specified
  * the BKDRHash function will be used.
  *
  * @param tbl the table to be initialized.
  * @param sz size to make the table.
  * @param hash function for hashing data when inserting or retrieving.
  * @param memfree function to free memory when destroying the hashtable.
  *
  * @return 0 on success, -1 on memory error, 1 if table is NULL, required
  * function is NULL or size is 0
  */
int ht_init(hashtbl_t * tbl, size_t sz, unsigned int (*hash) (unsigned char *),
            void (*memfree) (void *));


/** @brief deallocates memory for all elements and destroys a hashtable.
  *
  * @param tbl the table to be deallocated.
  */
void ht_destroy(hashtbl_t * tbl);

/** @brief adds an entry to the hashtable.  if an entry with the specified key
  * already exists, the value is overwritten.
  *
  * @param tbl hashtable in which the entry should be put
  * @param key string to use as the lookup key
  * @param data value to be stored.
  *
  * @return -1 on memory or 0 on success
  */
int ht_put(hashtbl_t * tbl, char *key, void *data);

/** @brief retrieves an entry's data from a hashtable.
  *
  * @param tbl table in which the data is stored
  * @param key string lookup key
  *
  * @return NULL if an element with the specified key does not exist, else
  * the data in the entry.
  */
void *ht_get(hashtbl_t * tbl, char *key);

/** @brief removes an entry from a hashtable
  *
  * @param tbl table in which the data is stored
  * @param key string lookup key
  */
void ht_delete(hashtbl_t * tbl, char *key);

/** @brief populates a list with the keys from the hashtable.
  *
  * @param tbl the hashtable
  * @param array an array to hold the list of keys.  Should be large enough to
  *              hold tbl->nelems pointers.
  * @return the number of keys stored in array.
  */
int ht_keys(hashtbl_t *tbl, char **array);

/** @brief executes a function for the data in each hashtable entry
  *
  * @param tbl table to be traversed
  * @param func function to call which takes the entry's data as its
  * only argument.
  */
void ht_call_for_each(hashtbl_t * tbl, void (*func) (void *));

/** @brief executes a function for the data in each hashtable entry
  *
  * @param tbl table to be traversed
  * @param func function to call which takes the entry's data as its
  * first argument.
  * @param data extra data to pass as the 2nd argument to func.
  */
void ht_call_for_each2(hashtbl_t * tbl, void (*func) (void *, void *),
                       void * data);

/** @brief prints some statistics for a hashtable useful for judging
  * hash algorithm performance.
  *
  * data is printed to sdterr and includes the allocated array size,
  * the number of empty cells, the average length of non-empty cells,
  * the maximum chain length, and the total number of elements in the
  * hashtable.
  *
  * @param tbl a hashtable
  */
void ht_dump_stats(hashtbl_t * tbl);


/** @brief gets the first prime number greater than or equal to N.
  *
  * @arg n some number.
  * @return a prime number not less than N.
  */
unsigned long ht_next_prime(unsigned long n);

#endif /* HASHTBL_H */

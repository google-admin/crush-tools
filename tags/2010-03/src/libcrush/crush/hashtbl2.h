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

/** @file hashtbl2.h
  * @brief Interface for the hashtbl2 library.
  *
  * This is a chained hashtable implementation, internally utilizing the
  * linklist library.  In this version, all key comparisons are performed
  * using memcmp().
  */

#include <stdlib.h>
#include <string.h>             /* strcmp(), strlen() */
#include <crush/linklist.h>           /* used internally */
#include <crush/ht2_GeneralHashFunctions.h>

#ifndef HASHTBL2_H
#define HASHTBL2_H

/** @brief the hashtable data type. */
typedef struct _hashtbl2 {
  size_t nelems;        /**< number of elements in the hashtable */
  size_t arrsz;       /**< size of the array */
  size_t keysz;       /**< size of the hash keys - for string keys, use libhashtbl instead. */
  llist_t **arr;        /**< array of linked lists */
  hash_function_t hash;     /**< hash function to use - see GeneralHashFunctions.h */
  void (*free) (void *);      /**< memory freeing function to call against an entry's data */
  int (*cmp) (void *, void *);    /**< key comparison function */
} hashtbl2_t;

/** @brief a key/value pair within the hashtable */
typedef struct _ht2_elem {
  void *key;  /**< lookup key for the element */
  void *data; /**< data to store in this element */
} ht2_elem_t;

/** @brief initializes a new hashtable.  the memfree function should be
  * specified iff the payload of a node will need to be deallocated when
  * the hashtable is destroyed.  if a NULL hash function is specified 
  * the BKDRHash function will be used.
  * 
  * @param tbl the table to be initialized.
  * @param sz size to make the table.
  * @param keysz size of the key data - for string keys, use libhashtbl instead.
  * @param hash function for hashing data when inserting or retrieving.
  * @param memfree function to free memory when destroying the hashtable.
  * @param cmp function to compare two keys.  should return zero when two keys are equal.
  *
  * @return 0 on success, -1 on memory error, 1 if table is NULL, required
  * function is NULL or size is 0
  */
int ht2_init(hashtbl2_t * tbl,
             size_t sz,
             size_t keysz,
             hash_function_t hash,
             void (*memfree) (void *), int (*cmp) (void *, void *)
  );


/** @brief deallocates memory for all elements and destroys a hashtable.
  * 
  * @param tbl the table to be deallocated.
  */
void ht2_destroy(hashtbl2_t * tbl);

/** @brief adds an entry to the hashtable.  if an entry with the specified key
  * already exists, the value is overwritten.
  * 
  * @param tbl hashtable in which the entry should be put
  * @param key string to use as the lookup key
  * @param data value to be stored.
  * 
  * @return -1 on memory or 0 on success
  */
int ht2_put(hashtbl2_t * tbl, void *key, void *data);

/** @brief retrieves an entry's data from a hashtable.
  * 
  * @param tbl table in which the data is stored
  * @param key string lookup key
  * 
  * @return NULL if an element with the specified key does not exist, else
  * the data in the entry.
  */
void *ht2_get(hashtbl2_t * tbl, void *key);

/** @brief removes an entry from a hashtable
  * 
  * @param tbl table in which the data is stored
  * @param key string lookup key
  */
void ht2_delete(hashtbl2_t * tbl, void *key);

/** @brief executes a function for the data in each hashtable entry
  * 
  * @param tbl table to be traversed
  * @param func function to call which takes the entry's data as its
  * only argument.
  */
void ht2_call_for_each(hashtbl2_t * tbl, int (*func) (void *));

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
void ht2_dump_stats(hashtbl2_t * tbl);

#endif /* HASHTBL2_H */

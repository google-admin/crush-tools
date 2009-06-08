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

#include <crush/hashtbl2.h>


/* initialize a table. */
int ht2_init(hashtbl2_t * tbl,
             size_t sz,
             size_t keysz,
             hash_function_t hash,
             void (*memfree) (void *), int (*cmp) (void *, void *)
  ) {

  /* some things are required */
  if (tbl == NULL || sz == 0 || keysz == 0 || cmp == NULL)
    return 1;

/** @todo find out if it's better to init all of the lists now or when they're needed.
 *	i suppose it depends on whether performance or memory usage is more important.
 *	from a performance standpoint, it would be better to do it now as a one-time
 *	cost rather than a comparison plus possible list initialization for every insert.
 */
  /* allocate memory for the table */
  if ((tbl->arr = malloc(sizeof(llist_t *) * sz)) == NULL)
    return -1;
  memset(tbl->arr, 0, sizeof(llist_t *) * sz);

  tbl->nelems = 0;
  tbl->arrsz = sz;
  tbl->keysz = keysz;
  tbl->cmp = cmp;
  tbl->free = memfree;          /* NULL ok here */

  /* set a default hash function if none specified */
  if (hash)
    tbl->hash = hash;
  else
    tbl->hash = ht2_BKDRHash;

  return 0;
}


/* destroy a table */
void ht2_destroy(hashtbl2_t * tbl) {
  int i;
  llist_node_t *listnode;

  /* loop through the array of linked lists */
  for (i = 0; i < tbl->arrsz; i++) {
    /* if there's anything in this list, loop through its
       members */
    if (tbl->arr[i] && tbl->arr[i]->nnodes) {
      for (listnode = tbl->arr[i]->head; listnode; listnode = listnode->next) {
        /* free the key (allocated from ht2_put) */
        free(((ht2_elem_t *) listnode->data)->key);
        /* free the data if appropriate */
        if (tbl->free)
          tbl->free(((ht2_elem_t *) listnode->data)->data);
      }
      /* deallocate the list & its elements */
      ll_destroy(tbl->arr[i]);
    }
    free(tbl->arr[i]);
  }
  free(tbl->arr);
  memset(tbl, 0, sizeof(hashtbl2_t));
}

/* put a new key/value pair into a table */
int ht2_put(hashtbl2_t * tbl, void *key, void *data) {
  unsigned long h;
  llist_node_t *listnode;
  ht2_elem_t *elem;

  if ((elem = malloc(sizeof(ht2_elem_t))) == NULL)
    return -1;
  if ((elem->key = malloc(tbl->keysz)) == NULL) {
    free(elem);
    return -1;
  }
  memcpy(elem->key, key, tbl->keysz);
  elem->data = data;

  /** @todo get rid of the modulo for better performance (if it matters) */
  h = tbl->hash(elem->key, tbl->keysz) % tbl->arrsz;


  if (!tbl->arr[h]) {
    tbl->arr[h] = malloc(sizeof(llist_t));
    ll_list_init(tbl->arr[h], free, NULL);
    /* using the normal free() function here because we just
       want the list to deallocate the ht2_elem_t data - we'll
       deallocate the inner data elements from here.
     */
    ll_add_elem(tbl->arr[h], elem, end);
    tbl->nelems++;
    return 0;
  }

  /* look for a matching key already in the hash */
  for (listnode = tbl->arr[h]->head; listnode; listnode = listnode->next)
    if (tbl->cmp(((ht2_elem_t *) listnode->data)->key, key) == 0)
      break;

  /* if no match found, insert the new one */
  if (!listnode) {
    ll_add_elem(tbl->arr[h], elem, end);
    tbl->nelems++;
  } else {
    /* match found: replace the old data with the new */
    if (tbl->free)
      tbl->free(((ht2_elem_t *) listnode->data)->data);
    listnode->data = elem;
    /* no size increment here */
  }
  return 0;
}

/* retrieve a value from a table */
void *ht2_get(hashtbl2_t * tbl, void *key) {
  unsigned long h;
  llist_t *list;
  llist_node_t *listnode;

  h = tbl->hash(key, tbl->keysz) % tbl->arrsz;
  list = tbl->arr[h];

  if (!list)                    /* invalid key - nothing in this slot yet */
    return NULL;

  /* find the node in the list with a matching key */
  for (listnode = list->head; listnode; listnode = listnode->next)
    if (tbl->cmp(((ht2_elem_t *) listnode->data)->key, key) == 0)
      break;
  if (!listnode)
    return NULL;
  return ((ht2_elem_t *) listnode->data)->data;
}

/* remove a key/value pair from a table */
void ht2_delete(hashtbl2_t * tbl, void *key) {
  unsigned long h;
  llist_t *list;
  llist_node_t *listnode;

  h = tbl->hash(key, tbl->keysz) % tbl->arrsz;
  list = tbl->arr[h];

  if (!list)                    /* invalid key - nothing in this slot yet */
    return;

  /* find the node in the list with a matching key */
  for (listnode = list->head; listnode; listnode = listnode->next)
    if (tbl->cmp(((ht2_elem_t *) listnode->data)->key, key) == 0)
      break;

  if (!listnode)                /* not found - do nothing */
    return;

  /* free the value data inside the node */
  tbl->free(((ht2_elem_t *) listnode->data)->data);
  /* free the node itself along with the key */
  ll_rm_elem(list, listnode);
  /* indicate that the hashtable is smaller now */
  tbl->nelems--;
}

/* execute some function for all of the elements in a table */
void ht2_call_for_each(hashtbl2_t * tbl, int (*func) (void *)) {
  int i;
  llist_node_t *node;
  for (i = 0; i < tbl->arrsz; i++) {
    if (tbl->arr[i] && tbl->arr[i]->nnodes > 0)
      for (node = tbl->arr[i]->head; node; node = node->next)
        func(((ht2_elem_t *) node->data)->data);
  }
}

/* print some population statistics for a table - useful for judging how well
   a hashing algorithm is performing.
 */
void ht2_dump_stats(hashtbl2_t * tbl) {
  size_t empty = 0;
  int avg_len = 0, maxlen = 0;
  int i;
  for (i = 0; i < tbl->arrsz; i++) {
    if ((!tbl->arr[i]) || tbl->arr[i]->nnodes == 0)
      empty++;
    else {
      avg_len += tbl->arr[i]->nnodes;
      if (tbl->arr[i]->nnodes > maxlen)
        maxlen = tbl->arr[i]->nnodes;
    }
  }
  if (tbl->arrsz != empty)
    avg_len = avg_len / (tbl->arrsz - empty);
  fprintf(stderr,
          "size:\t%zd\nempty:\t%zd\naverage length (nonempty only): %d\nmax length:\t%d\ntotal elems:\t%zd\n",
          tbl->arrsz, empty, avg_len, maxlen, tbl->nelems);
}

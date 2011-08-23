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

#include <stdio.h>
#include <crush/general.h>
#include <crush/hashtbl.h>


static int ht_key_cmp(const void *a, const void *b) {
  return strcmp(((ht_elem_t *) a)->key,
                ((ht_elem_t *) b)->key);
}

static void ht_free_tree_data(void (*free_fn) (void *), bst_node_t *node) {
  if (! node)
    return;
  ht_free_tree_data(free_fn, node->r);
  ht_free_tree_data(free_fn, node->l);
  free_fn(((ht_elem_t *) node->data)->data);
}

static int ht_rehash_2x(hashtbl_t *tbl);

/* initialize a table. */
int ht_init(hashtbl_t * tbl,
            size_t sz,
            unsigned int (*hash) (unsigned char *),
            void (*memfree) (void *)) {

  /* some things are required */
  if (tbl == NULL || sz == 0)
    return 1;

  sz = ht_next_prime(sz);

  tbl->arr = xmalloc(sizeof(bstree_t *) * sz);
  memset(tbl->arr, 0, sizeof(bstree_t *) * sz);

  tbl->ht_elem_pool = mempool_create(sizeof(ht_elem_t) * 128);
  if (tbl->ht_elem_pool == NULL)
    return -1;

  /* since keys are free-form text, the pool size is arbitrary. */
  tbl->key_pool = mempool_create(4096);
  if (tbl->key_pool == NULL)
    return -1;

  tbl->nelems = 0;
  tbl->arrsz = sz;
  tbl->free = memfree;  /* NULL ok here */
  if (hash)             /* set a default hash function if none specified */
    tbl->hash = hash;
  else
    tbl->hash = BKDRHash;

  return 0;
}


/* destroy a table */
void ht_destroy(hashtbl_t * tbl) {
  int i;

  /* Loop through the array of trees, freeing data if necessary, then
   * deallocating the tree. */
  for (i = 0; i < tbl->arrsz; i++) {
    if (tbl->arr[i]) {
      if (tbl->free)
        ht_free_tree_data(tbl->free, tbl->arr[i]->root);
      bst_destroy(tbl->arr[i]);
    }
    free(tbl->arr[i]);
  }
  free(tbl->arr);
  mempool_destroy(tbl->ht_elem_pool);
  mempool_destroy(tbl->key_pool);
  memset(tbl, 0, sizeof(hashtbl_t));
}


/* Put a new key/value pair into a table. */
int ht_put(hashtbl_t * tbl, char *key, void *data) {
  unsigned long h;
  bst_node_t *treenode;
  ht_elem_t *elem, key_elem;

  key_elem.key = key;
  elem = mempool_alloc(tbl->ht_elem_pool, sizeof(ht_elem_t));
  if (! elem)
    return -1;
  elem->key = mempool_alloc(tbl->key_pool,
                            sizeof(char) * strlen(key) + 1);
  if (! elem->key) {
    /* elem leaks here, but we cannot free it from the mempool. */
    return -1;
  }
  strcpy(elem->key, key);
  elem->data = data;

  h = tbl->hash((unsigned char *) elem->key) % tbl->arrsz;

  if (!tbl->arr[h]) {
    tbl->arr[h] = xmalloc(sizeof(bstree_t));
    /* No free() fn for the bst, since its elements are in a mempool. */
    bst_init(tbl->arr[h], ht_key_cmp, NULL);
    bst_insert(tbl->arr[h], elem);
    tbl->nelems++;
    return 0;
  }

  treenode = bst_find(tbl->arr[h], &key_elem);

  /* If no match is found, insert the new element and increase the counter.
   * Otherwise, replace the old data with the new. */
  if (!treenode) {
    bst_insert(tbl->arr[h], elem);
    tbl->nelems++;
  } else {
    if (tbl->free)
      tbl->free(((ht_elem_t *) treenode->data)->data);
    treenode->data = elem;
  }

  if (tbl->nelems > tbl->arrsz)
    return ht_rehash_2x(tbl);

  return 0;
}


/* retrieve a value from a table */
void *ht_get(hashtbl_t * tbl, char *key) {
  unsigned long h;
  bstree_t *tree;
  bst_node_t *treenode;
  ht_elem_t key_elem;

  h = tbl->hash((unsigned char *) key) % tbl->arrsz;
  tree = tbl->arr[h];

  if (! tree)  /* invalid key - nothing in this slot yet */
    return NULL;
  key_elem.key = key;
  treenode = bst_find(tree, &key_elem);
  if (! treenode)
    return NULL;
  return ((ht_elem_t *) treenode->data)->data;
}


/* remove a key/value pair from a table */
void ht_delete(hashtbl_t * tbl, char *key) {
  unsigned long h;
  bstree_t *tree;
  bst_node_t *treenode;
  ht_elem_t key_elem;

  h = tbl->hash((unsigned char *) key) % tbl->arrsz;
  tree = tbl->arr[h];

  if (! tree)  /* A NULL slot means the key is unknown. */
    return;
  key_elem.key = key;
  treenode = bst_find(tree, &key_elem);
  if (treenode) {
    if (tbl->free)
      tbl->free(((ht_elem_t *) treenode->data)->data);
    bst_delete(tree, &key_elem);
    tbl->nelems--;
  }
}


/* context for element rehashing */
static struct {
  hashtbl_t *tbl;
  size_t newsz;
  bstree_t **newarr;
} rehash_binding;


/* rehash an element to the new HT */
static void ht_rehash_elem(ht_elem_t *elem) {
  unsigned long h;
  if (rehash_binding.tbl == NULL || rehash_binding.newarr == NULL)
    return;

  h = rehash_binding.tbl->hash((unsigned char *)elem->key) %
      rehash_binding.newsz;
  if (!rehash_binding.newarr[h]) {
    rehash_binding.newarr[h] = xmalloc(sizeof(bstree_t));
    bst_init(rehash_binding.newarr[h], ht_key_cmp, NULL);
  }
  bst_insert(rehash_binding.newarr[h], elem);
}


/* grow the hash table */
static int ht_rehash_2x(hashtbl_t *tbl) {
  int i;
  if (tbl == NULL)
    return 1;

#ifdef CRUSH_DEBUG
  fprintf(stderr, "rehashing ... ");
#endif
  memset(&rehash_binding, 0, sizeof(rehash_binding));
  rehash_binding.tbl = tbl;
  rehash_binding.newsz = ht_next_prime(tbl->arrsz * 2);
  rehash_binding.newarr = xmalloc(sizeof(bstree_t *) * rehash_binding.newsz);
  memset(rehash_binding.newarr, 0, sizeof(bstree_t *) * rehash_binding.newsz);
  for (i = 0; i < tbl->arrsz; i++) {
    if (tbl->arr[i]) {
      bst_call_for_each(tbl->arr[i], (void(*)(void*))ht_rehash_elem, preorder);
      bst_destroy(tbl->arr[i]);
      free(tbl->arr[i]);
    }
  }
  free(tbl->arr);
  tbl->arrsz = rehash_binding.newsz;
  tbl->arr = rehash_binding.newarr;
#ifdef CRUSH_DEBUG
  fprintf(stderr, "done, new size: %d\n", tbl->arrsz);
#endif
  return 0;
}


static void ht_keys_bst_traverse(bst_node_t *node,
                                 char **array,
                                 int *index) {
  if (! node)
    return;
  array[*index] = ((ht_elem_t *) node->data)->key;
  (*index)++;
  ht_keys_bst_traverse(node->l, array, index);
  ht_keys_bst_traverse(node->r, array, index);
}


int ht_keys(hashtbl_t *tbl, char **array) {
  int i, j = 0;
  for (i = 0; i < tbl->arrsz; i++) {
    if (tbl->arr[i])
      ht_keys_bst_traverse(tbl->arr[i]->root, array, &j);
  }
  return j;
}


static void ht_call_bst_traverse(bst_node_t *node, void (*func) (void *)) {
  if (! node)
    return;
  func(((ht_elem_t *)node->data)->data);
  ht_call_bst_traverse(node->l, func);
  ht_call_bst_traverse(node->r, func);
}


static void ht_call_bst_traverse2(bst_node_t *node,
                                  void (*func) (void *, void *),
                                  void * data) {
  if (! node)
    return;
  func(((ht_elem_t *)node->data)->data, data);
  ht_call_bst_traverse2(node->l, func, data);
  ht_call_bst_traverse2(node->r, func, data);
}


/* Execute some function for all of the elements in a table. */
void ht_call_for_each(hashtbl_t * tbl, void (*func) (void *)) {
  int i;
  for (i = 0; i < tbl->arrsz; i++) {
    if (tbl->arr[i])
      ht_call_bst_traverse(tbl->arr[i]->root, func);
  }
}


void ht_call_for_each2(hashtbl_t * tbl, void (*func) (void *, void *),
                       void * data) {
  int i;
  for (i = 0; i < tbl->arrsz; i++) {
    if (tbl->arr[i])
      ht_call_bst_traverse2(tbl->arr[i]->root, func, data);
  }
}


/* Print some population statistics for a table - useful for judging how well
   a hashing algorithm is performing.
 */
void ht_dump_stats(hashtbl_t * tbl) {
  size_t uninitialized = 0;
  int i;
  for (i = 0; i < tbl->arrsz; i++) {
    if ((!tbl->arr[i])) {
      uninitialized++;
    }
  }

  /** @todo Provide average bucket-fill metric here. */
  fprintf(stderr,
          "size:\t%lu\nuninitialized buckets:\t%lu\nelements:\t%lu",
          tbl->arrsz, uninitialized, tbl->nelems);
}


static int ht_is_prime(unsigned long n) {
  static int primes[] = {
#include "primes.dat"
  };
  int n_primes = sizeof(primes) / sizeof(int);
  unsigned long half_n = n / 2;
  int i;
  for (i=0; i < n_primes; i++) {
    if (n % primes[i] == 0)
      return 0;
    if (primes[i] > half_n)
      return 1;
  }
  return 1;
}


unsigned long ht_next_prime(unsigned long n) {
  while (! ht_is_prime(n))
    n++;
  return n;
}

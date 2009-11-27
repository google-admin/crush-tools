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
#include <sys/types.h>
#include <assert.h>
#include <locale.h>

#include <crush/ffutils.h>
#include <crush/bstree.h>
#include <crush/mempool.h>
#include <crush/linklist.h>

#ifndef TREESORT_H
#define TREESORT_H

struct tsort_conf {
	int *keys, *rest;
	ssize_t keys_ct, rest_ct, keys_sz;
	traversal_order_t *order;

	char unique;

	bstree_t tree;
	mempool_t *tr_pool;
	mempool_t *key_pool;
};

typedef struct _stree_t {
	union {
		bstree_t tree;
		llist_t list;
	};
	char keylevel;

	char *key;
} stree_t;

void print_data(stree_t *t);
int key_strcmp(const stree_t *a, const stree_t *b);
void stree_free(stree_t *t);
int tsort_init(struct tsort_conf *conf);
void tsort_destroy(struct tsort_conf *conf);
void fputs_stree(stree_t *s);

#endif /* TREESORT_H */

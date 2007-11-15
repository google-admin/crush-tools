#include <hashtbl.h>


/* initialize a table. */
int ht_init(	hashtbl_t *tbl,
		size_t sz,
		unsigned int (* hash)(unsigned char *),
		void (* memfree)(void *) ) {

	/* some things are required */
	if ( tbl == NULL || sz == 0 )
		return 1;

/** @todo find out if it's better to init all of the lists now or when they're needed.
 *	i suppose it depends on whether performance or memory usage is more important.
 *	from a performance standpoint, it would be better to do it now as a one-time
 *	cost rather than a comparison plus possible list initialization for every insert.
 */
	/* allocate memory for the table */
	if ( (tbl->arr = malloc( sizeof( llist_t * ) * sz )) == NULL )
		return -1;
	memset(tbl->arr, 0, sizeof( llist_t * ) * sz );

	tbl->nelems = 0;
	tbl->arrsz = sz;
	tbl->free = memfree;	/* NULL ok here */
	if ( hash )		/* set a default hash function if none specified */
		tbl->hash = hash;
	else
		tbl->hash = BKDRHash;

	return 0;
}


/* destroy a table */
void ht_destroy( hashtbl_t *tbl ){
	int i;
	llist_node_t *listnode;

	/* loop through the array of linked lists */
	for (i = 0; i < tbl->arrsz; i++ ) {
		/* if there's anything in this list, loop through its
		   members */
		if ( tbl->arr[i] && tbl->arr[i]->nnodes ){
			for(listnode = tbl->arr[i]->head; listnode; listnode = listnode->next) {
				/* free the key (allocated from ht_put)*/
				free( ((ht_elem_t *)listnode->data)->key );
				/* free the data if appropriate */
				if ( tbl->free )
					tbl->free( ((ht_elem_t *)listnode->data)->data );
			}
			/* deallocate the list & its elements */
			ll_destroy( tbl->arr[i] );
		}
		free( tbl->arr[i] );
	}
	free(tbl->arr);
	memset(tbl, 0, sizeof(hashtbl_t));
}

/* put a new key/value pair into a table */
int ht_put( hashtbl_t *tbl, char *key, void *data ) {
	unsigned long h;
	llist_node_t *listnode;
	ht_elem_t *elem;

	if ( (elem = malloc(sizeof(ht_elem_t))) == NULL )
		return -1;
	if ( (elem->key = malloc( sizeof(char) * strlen(key) + 1 )) == NULL ) {
		free(elem);
		return -1;
	}
	strcpy(elem->key, key);
	elem->data = data;

	/** @todo get rid of the modulo for better performance (if it matters) */
	h = tbl->hash(elem->key) % tbl->arrsz;

	
	if ( ! tbl->arr[h] ) {
		tbl->arr[h] = malloc(sizeof(llist_t));
		ll_list_init( tbl->arr[h], free, NULL );
		/* using the normal free() function here because we just
		   want the list to deallocate the ht_elem_t data - we'll
		   deallocate the inner data elements from here.
		 */
		ll_add_elem( tbl->arr[h], elem, end );
		tbl->nelems++;
		return 0;
	}

	/* look for a matching key already in the hash */
	for ( listnode = tbl->arr[h]->head; listnode; listnode = listnode->next )
		if ( strcmp( ((ht_elem_t *) listnode->data)->key, key ) == 0 )
			break;

	/* if no match found, insert the new one */
	if ( ! listnode ) {
		ll_add_elem( tbl->arr[h], elem, end );
		tbl->nelems++;
	} else {
		/* match found: replace the old data with the new */
		if ( tbl->free )
			tbl->free( ((ht_elem_t *)listnode->data)->data );
		listnode->data = elem;
		/* no size increment here */
	}
	return 0;
}

/* retrieve a value from a table */
void * ht_get( hashtbl_t *tbl, char *key ) {
	unsigned long h;
	llist_t *list;
	llist_node_t *listnode;

	h = tbl->hash(key) % tbl->arrsz;
	list = tbl->arr[h];

	if ( ! list )	/* invalid key - nothing in this slot yet */
		return NULL;

	/* find the node in the list with a matching key */
	for ( listnode = list->head; listnode; listnode = listnode->next )
		if ( strcmp( ((ht_elem_t *) listnode->data)->key, key ) == 0 )
			break;
	if ( ! listnode )
		return NULL;
	return ((ht_elem_t *) listnode->data)->data;
}

/* remove a key/value pair from a table */
void ht_delete( hashtbl_t *tbl, char *key ) {
	unsigned long h;
	llist_t *list;
	llist_node_t *listnode;

	h = tbl->hash(key) % tbl->arrsz;
	list = tbl->arr[h];

	if ( ! list )	/* invalid key - nothing in this slot yet */
		return;

	/* find the node in the list with a matching key */
	for ( listnode = list->head; listnode; listnode = listnode->next )
		if ( strcmp( ((ht_elem_t *) listnode->data)->key, key ) == 0 )
			break;

	if ( ! listnode ) /* not found - do nothing */
		return;

	/* free the value data inside the node */
	tbl->free( ((ht_elem_t *)listnode->data)->data );
	/* free the node itself along with the key string */
	ll_rm_elem( list, listnode );
	/* indicate that the hashtable is smaller now */
	tbl->nelems--;
}

/* execute some function for all of the elements in a table */
void ht_call_for_each( hashtbl_t *tbl, int (* func)( void * ) ) {
	int i;
	llist_node_t *node;
	for ( i = 0; i < tbl->arrsz; i++ ) {
		if ( tbl->arr[i] && tbl->arr[i]->nnodes > 0 )
			for( node = tbl->arr[i]->head; node; node = node->next)
				func( ((ht_elem_t *)node->data)->data );
	}
}

/* print some population statistics for a table - useful for judging how well
   a hashing algorithm is performing.
 */
void ht_dump_stats( hashtbl_t *tbl ) {
	size_t empty = 0;
	int avg_len = 0, maxlen = 0;
	int i;
	for ( i = 0; i < tbl->arrsz; i++ ) {
		if ( ( ! tbl->arr[i] ) || tbl->arr[i]->nnodes == 0 )
			empty++;
		else {
			avg_len += tbl->arr[i]->nnodes;
			if ( tbl->arr[i]->nnodes > maxlen )
				maxlen = tbl->arr[i]->nnodes;
		}
	}
	if ( tbl->arrsz != empty )
		avg_len = avg_len / (tbl->arrsz - empty);
	fprintf(stderr, "size:\t%zd\nempty:\t%zd\naverage length (nonempty only): %d\nmax length:\t%d\ntotal elems:\t%zd\n",
			tbl->arrsz, empty, avg_len, maxlen, tbl->nelems);
}



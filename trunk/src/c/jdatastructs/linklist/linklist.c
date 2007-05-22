#include <linklist.h>

/* see linklist.h for better function-level documentation */

/* initialize a new linked list */
int ll_list_init( llist_t *list,
			void (* data_free)(void *),
			int (* data_cmp)(const void *, const void *)) {
	list->nnodes = 0;
	list->head = list->tail = NULL;
	list->free = data_free;
	if ( data_cmp )
		list->cmp = data_cmp;
	return 0;
}

/* allocate & initialize a new list node */
int ll_init_node( llist_node_t **node, void *data ) {
	if ( (*node = malloc(sizeof(llist_node_t))) == NULL )
		return -1;
	(*node)->prev = (*node)->next = NULL;
	(*node)->data = data;
	return 0;
}

/* free all nodes in a list and null out its properties */
int ll_destroy ( llist_t *list ) {
	if ( list == NULL || list->head == NULL )
		return 0;

	while ( list->head )
		ll_rm_elem( list, list->head );

	list->free = NULL; list->cmp = NULL;
	return 0;
}

/* remove an element/node from a list, freeing its data if appropriate */
int ll_rm_elem ( llist_t *list, llist_node_t *node ) {
	int ret = 0;

	if ( list == NULL || node == NULL )
		return 0;

	/* special cases if removing the head and/or tail of the list */
	if ( node == list->head )
		list->head = node->next;
	if ( node == list->tail )
		list->tail = node->prev;

	/* if this node has a *prev, point its *next around the
	   one to be removed */
	if ( node->prev )
		node->prev->next = node->next;
	/* if this node has a *next, point its *prev around the
	   one to be removed */
	if ( node->next )
		node->next->prev = node->prev;

	#ifdef DEBUG
	fprintf(stderr, "freeing node data \"%s\" at %p\n", node->data, node->data);
	#endif
	
	if ( list->free )	/* free the payload if a function was given */
		list->free(node->data);

	#ifdef DEBUG
	fprintf(stderr, "freeing node at %p\n", node);
	#endif
	
	free(node);	/* free the node structure itself */

	list->nnodes--;	/* indicate that there's now 1 fewer nodes in the list */
	return ret;
}

/* add a new element to a list in some particular relative position */
int ll_add_elem( llist_t *list, void *data, insert_point_t at ) {
	llist_node_t *newnode;

	/* asking for sorted insertion, but no function for comparing
	   the data was given */
	if ( at == sorted && ! list->cmp )
		return -1;

	/* return if we fail to create a new node */
	if ( ll_init_node( &newnode, data ) == -1 )
		return -1;

	if ( at == beginning || list->head == NULL ) {
		/* this will be the first node in the list */
		newnode->next = list->head;
		list->head = newnode;
		/* if the list was empty before, make the new node the
		   tail also */
		if ( ! newnode->next )
			list->tail = newnode;

	} else if ( at == end ) {
		/* this will be the last node in the list */

		/* we know the list already has a tail because the first
		   condition (head isn't null) tested false this time */

		list->tail->next = newnode;
		newnode->prev = list->tail;
		list->tail = newnode;

	} else if ( at == sorted ) {
		/* use list->cmp function to insert the element
		   in sorted order */
		llist_node_t *cur;

		/* get to the first node that's greater than the new one */
		for ( cur = list->head; cur; cur = cur->next ) {
			if ( list->cmp( cur->data, data ) > 0 )
				break;
		}

		/* (the case of (list->head == NULL) is caught above
		   in (at == beginning) ) */
		if ( cur == list->head ) {
			/* put the new node in front */
			newnode->next = list->head;
			list->head = newnode;
		} else if ( cur != NULL ) {
			/* put the new node in the middle somewhere */
			newnode->next = cur;
			newnode->prev = cur->prev;
			cur->prev = newnode;
		} else {
			/* put the new node at the end */
			newnode->prev = list->tail;
			list->tail->next = newnode;
			list->tail = newnode;
		}
	}
	list->nnodes++;	/* indicate that the list now has one more node */
	return 0;
}

/* ***
 * these two are currently implemented as macros.  making them into 
 * functions as below would make them typesafe at a minor performance
 * expense.
int ll_prepend_elem( llist_t *list, void *data ) {
	return ll_add_elem(list, data, beginning);
}

int ll_append_elem( llist_t *list, void *data ) {
	return ll_add_elem(list, data, end);
}
*** */

/* converts the linked list into an array of nodes.  array must already
   be allocated to >= list->nnodes.  not used in phonebk v1.0 */
void ll_to_array ( llist_t *list, llist_node_t **array ) {
	llist_node_t *cur;
	size_t n = 0;
	for ( cur = list->head; cur; cur = cur->next )
		array[n++] = cur;
	return;
}

/* call a function for each element of the list.  the function should
   take the current node's data as its only argument */
int ll_call_for_each ( llist_t *list, int (* func)( void * ) ) {
	llist_node_t *node;
	int ret = 0;

	/* make sure this wasn't called with a null list or function  */
	if ( list == NULL || func == NULL )
		return -1;

	for ( node = list->head; node; node = node->next )
		ret = func(node->data);
	return ret;
}


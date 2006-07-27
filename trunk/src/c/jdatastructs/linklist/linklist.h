/** @file linklist.h
  * @brief Interface for the doubly-linked list library.
  */

/** @mainpage
  * This is a library which provides doubly-linked list functionality.  It was 
  * written for Regis University course CS437, Advanced File and Data Structures.
  *
  * @section copy Author and Copyright
  * Written by Jeremy Hinds <jeremy.hinds@gmail.com>
  *
  * &copy; 2005, Jeremy Hinds
  *
  * @section Links
  * <a href="../../index.html">Back to the main docs page</a>
  */
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>	/* memcmp() */
#include <stdio.h>	/* printf() in ll_dump() */

#ifndef LINKLIST_H
#define LINKLIST_H

/** @brief indicator used by ll_add_elem for where in the list to put the new
  * member. */
typedef enum {beginning, end, sorted} insert_point_t;

/** @brief a node in a linked list. */
typedef struct _llist_node_t {
	struct _llist_node_t *next;	/**< next node in the list */
	struct _llist_node_t *prev;	/**< previous node in the list */
	void *data;			/**< the payload */
} llist_node_t;


/** @brief a linked list handle. */
typedef struct _llist_t {
	llist_node_t *head;			/**< head of the list */
	llist_node_t *tail;			/**< tail of the list */
	size_t nnodes;				/**< number of nodes in list */
	void (* free)(void *);			/**< function to free data from the list */
	int (* cmp)(	const void *,
			const void *);		/**< function to compare two nodes */
} llist_t;

/** @brief returns the number of elements in a linked list
  * @param l the list
  */
#define ll_list_size( l ) \
	(l)->nnodes;

/** @brief adds an element to the beginning of a linked list
  * 
  * @param list list to which the element should be added
  * @param data payload to put in the new list node
  * 
  */
#define ll_prepend_elem( list, data ) \
	ll_add_elem((list), (data), beginning)

/** @brief adds an element to the end of a linked list
  * 
  * @param list list to which the element should be added
  * @param data payload to put in the new list node
  * 
  */
#define ll_append_elem( list, data ) \
	ll_add_elem((list), (data), end);

/** @brief initializes a linked list handle
  * 
  * @param list the list handle to be initialized
  * @param data_free function to free the data memory of a node - NULL if it shouldn't be deallocated
  * @param data_cmp function to compare the data of two nodes when inserting nodes in sorted order - default is memcmp()
  *
  * the arguments of data_cmp are like those of memcmp or strncmp, as is the expectation of the return values.
  * the lesser of the two data sizes is passed as the last argument.
  * 
  * @return 0 on success
  */
int ll_list_init( llist_t *list,
			void (* data_free)(void *),
			int (* data_cmp)(const void *, const void *)) ;

/** @brief allocates a node and initializes its fields
  * 
  * @param node unallocated node
  * @param data payload for the new node
  *
  * @return 0 on success, -1 on memory error
  */
int ll_init_node( llist_node_t **node, void *data ) ;


/** @brief frees all of the nodes in a linked list
  * 
  * @param head first element of the list
  * 
  * @return 
  */
int ll_destroy( llist_t *head ) ;


/** @brief  
  * 
  * @param list the list to which the element should be added
  * @param data payload to put in the new list node
  * @param at where in the list the new node should be put
  * 
  * @return 0 on success, -1 on memory error
  */
int ll_add_elem( llist_t *list, void *data, insert_point_t at ) ;


/** @brief removes a node from a list
  * 
  * @param list 
  * @param node the one to be removed
  * 
  * @return 0 on success, nonzero on error freeing memory
  */
int ll_rm_elem( llist_t *list, llist_node_t *node ) ;

/** @brief puts node pointers into an (already allocated) array.  (this
  * function isn't currently used in the phonebk application.)
  * 
  * @param list the list
  * @param array array with at least ll_list_size(list) elements allocated
  */
void ll_to_array ( llist_t *list, llist_node_t **array ) ;

/** @brief call a function for the data in each element in a linked list.
  * the function will be passed the data in each node of the list.
  * 
  * @param list the list to be iterated through
  * @param func the function to call
  * 
  * @return the last return value of the executed function, or -1 if
  * the list or the function pointer were NULL.
  */
int ll_call_for_each ( llist_t *list, int (* func)( void * ) ) ;


#endif /* LINKLIST_H */


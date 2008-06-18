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

/** @file bstree.h
  * @brief interface for the binary search tree library.
  *
  * This is a library which provides binary search tree functionality.
  */

#include <stdlib.h>	/* malloc(), free() */
#include <string.h>	/* memset() */
#include <queue.h>	/* for the breadth-first traversal */

#ifndef BSTREE_H
#define BSTREE_H

/** @brief specifies a tree traversal order */
typedef enum {	preorder,	/**< traverse the tree preorder */
		inorder,	/**< traverse the tree inorder */
		postorder,	/**< traverse the tree postorder */
		breadthfirst	/**< traverse the tree breadth-first */
} traversal_order_t;

/** @brief indicates the balance status of a binary tree node.
  * While a node's balance factor is visible to the user, it is
  * only used internally to keep the tree balanced during inserts
  * and deletes.
  */
typedef enum {	leftheavy,	/**< indicates that the node needs rotated right. */
		balanced,	/**< indicates that the node is in balance */
		rightheavy	/**< indicates that the node needs rotated left. */
} node_balance_t;

/** @brief a node in a binary search tree. */
typedef struct _bst_node {
	struct _bst_node *l;		/**< @brief child to the left.
					     the left child's data is equal to
					     or less than this node. */
	struct _bst_node *r;		/**< @brief child to the right.
					     the right child's data is greater
					     than this node. */
	struct _bst_node *parent;	/**< @brief parent of this node. */
	void *data;			/**< @brief payload data. */
	node_balance_t balance;		/**< @brief the balance status of this node. */
} bst_node_t;

/** @brief binary search tree data type. */
typedef struct _bstree {
	bst_node_t *root;	/**< @brief root node of the tree. */
	int (* cmp)(const void *, const void *);	/**< @brief function for comparing node data.  return values are like those for strcmp() */
	void (* free)(void *);		/**< @brief function to free node data. */
} bstree_t;

/** @brief initializes a binary search tree.  the tree should be allocated
  * already.
  *
  * @param tree an allocated, but otherwise uninitialized binary search tree.
  * @param cmp function to compare node data, where a return value of 0 indicates
  * equality, < 0 means the first param is less than the second, and > 0 means
  * that the first param is greater than the second.
  * @param free function to free node data when the tree is destroyed.  specify
  * NULL if the data should not be freed.
  *
  * @return 0 on success, or -1 if cmp is NULL.
  */
int bst_init(	bstree_t *tree,
		int (* cmp)(const void *, const void *),
		void (* free)(void *) );

/** @brief frees all memory associated with a binary search tree.
  *
  * @param tree the tree to be destroyed.
  */
void bst_destroy( bstree_t *tree );

/** @brief creates a child of the specified parent node containing the given data.
  *
  * @param tree the binary search tree in which the parent and child reside.
  * @param parent the parent of the new node, or NULL if the new node will be the tree's root.
  * @param data the data to store in the new node.
  *
  * @return NULL if the parent node already has a child in the direction where
  * the new node would go or if there is a memory error, else a pointer to the
  * new node.
  */
bst_node_t * bst_create_child( bstree_t *tree, bst_node_t *parent, void *data ) ;

/** @brief inserts a node containing specified data into a binary search tree.
  *
  * @param tree the tree in which the data should be inserted.
  * @param data the data to insert.
  *
  * @return a pointer to the new node
  */
bst_node_t * bst_insert( bstree_t *tree, void *data );

/** @brief deletes a node containing specified data from a binary search tree.
  *
  * @param tree the tree from which the data should be deleted.
  * @param data the data to find and remove.
  */
void bst_delete( bstree_t *tree, void *data );

/** @brief finds the node in a binary search tree containing the specified data.
  * @param tree the tree contianing the data.
  * @param data the data to be found.
  * @return the node containing the specified data, or NULL if not found.
  */
bst_node_t * bst_find( bstree_t *tree, void *data );

/** @brief traverses a binary search tree in the indicated order, calling the specified
  * function for each data item in the tree.
  *
  * @param tree the tree to be traversed.
  * @param func the function to be executed.
  * @param order the order in which the tree should be traversed.
  */
void bst_call_for_each( bstree_t *tree, void (* func)( void * ), traversal_order_t order );

/** @brief traverses a (sub)tree in the indicated order, calling the specified
  * function for each data item in the tree.
  *
  * @param node root of the (sub)tree to be traversed.
  * @param func the function to be executed.
  * @param order the order in which the tree should be traversed.
  */
void bstn_call_for_each( bst_node_t *node, void (* func)( void * ), traversal_order_t order );


#endif /* BSTREE_H */


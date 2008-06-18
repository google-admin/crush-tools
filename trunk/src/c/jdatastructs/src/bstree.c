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

/*
 * implementation of binary search tree functions.  see bstree.h for
 * better function-level documentation.
 */

#include <bstree.h>

#ifdef DEBUG
#include <stdio.h>	/* only needed for debug prints */
#endif

/* ****************************** *
 * non-public function prototypes *
 * ****************************** */

/* this only frees the node itself - not the node data */
void bst_destroy_nodes( bst_node_t *node );

/* 2005-07-29 - not implemented yet */
void bst_rotate_right( bstree_t *tree, bst_node_t *node );
void bst_rotate_left( bstree_t *tree, bst_node_t *node );

/* these do all the real work of the *_call_for_each() functions */
void bstn_call_preorder( bst_node_t *node, void (* func)(void *) );
void bstn_call_inorder( bst_node_t *node, void (* func)(void *) );
void bstn_call_postorder( bst_node_t *node, void (* func)(void *) );
void bstn_call_breadthfirst ( bst_node_t *node, void (* func)(void *) );


/* ****************************** *
 * function definitions           *
 * ****************************** */

/* initialize a binary tree */
int bst_init(	bstree_t *tree,
		int (* cmp)(const void *, const void *),
		void (* free)(void *) ){
	if ( ! cmp )
		return -1;
	tree->root = NULL;
	tree->cmp = cmp;
	tree->free = free;
	return 0;
}

/* free all of the data in a tree, and then free the nodes */
void bst_destroy( bstree_t *tree ){
	if ( tree->free )
		bstn_call_for_each( tree->root, tree->free, inorder );
	bst_destroy_nodes(tree->root);
	memset( tree, 0, sizeof(bstree_t) );
}

/* free memory  all the nodes in a tree - called from bst_destroy */
void bst_destroy_nodes( bst_node_t *node ) {
	if ( ! node )
		return;
	bst_destroy_nodes ( node->l );
	bst_destroy_nodes ( node->r );
	free( node );
}

/* allocate a node, set its data, and make it a child of the specified parent */
bst_node_t * bst_create_child( bstree_t *tree, bst_node_t *parent, void *data ) {
	bst_node_t *new;

	int cmp = 0;
	if ( parent ) {
		/* the parent is specified - won't be the root node */
		cmp = tree->cmp(data, parent->data);
		if ( cmp > 0 && parent->r != NULL ) {
			/* new data greater than parent's data */
#ifdef DEBUG
			fprintf(stderr, "parent already has right node.");
#endif
			return NULL;
		} else if ( cmp <= 0 && parent->l != NULL ) {
			/* new data less than/equal to parent's data */
#ifdef DEBUG
			fprintf(stderr, "parent already has left node.");
#endif
			return NULL;
		}
	} else if ( tree->root ) {
		/* no parent, but the tree already has a root! */
#ifdef DEBUG
		fprintf(stderr, "tree already has a root - parent must be specified.");
#endif
		return NULL;
	}

	new = malloc(sizeof(bst_node_t));
	if ( ! new )
		return NULL;

	new->data = data;
	new->l = new->r = NULL;
	new->balance = balanced;

	if ( parent ) {
		new->parent = parent;
		if ( cmp > 0 ) {
			parent->r = new;
			if ( ! parent->l )
				parent->balance = rightheavy;
			else
				parent->balance = balanced;
		} else {
			parent->l = new;
			if ( ! parent->r )
				parent->balance = leftheavy;
			else
				parent->balance = balanced;
		}

		/** @bug rotation to rebalance the tree upon node insertion is currently broken */

		/* 

		if ( parent->parent ) {
			if ( parent->balance == leftheavy && parent->parent->balance == leftheavy )
				bst_rotate_left(tree, parent->parent);
			else if ( parent->balance == rightheavy && parent->parent->balance == rightheavy )
				bst_rotate_right(tree, parent->parent);
		}

		*/
	} else {
		tree->root = new;
		new->parent = NULL;
	}
	return new;
}

/* insert a node into a tree */
bst_node_t * bst_insert( bstree_t *tree, void *data ){
	bst_node_t *cur;
	for ( cur = tree->root; cur; ) {
		if ( tree->cmp( data, cur->data ) > 0 ) {
			if ( ! cur->r )
				break;
			cur = cur->r;
		} else {
			if ( ! cur->l )
				break;
			cur = cur->l;
		}
	}
	return bst_create_child( tree, cur, data ) ;
}

/* delete from a tree the node containing specified data */
void bst_delete( bstree_t *tree, void *data ){
	bst_node_t *cur;

	cur = bst_find( tree, data );

	if ( ! cur ) {
#ifdef DEBUG
		fprintf(stderr, "node not found\n");
#endif
		return;
	}

	if ( cur->l == NULL && cur->r == NULL) {
		/* leaf node - just delete it and null out the parent pointer */
		if ( cur->parent ) {
			if ( cur->parent->l == cur )
				cur->parent->l = NULL;
			else if ( cur->parent->r == cur )
				cur->parent->r = NULL;
		} else {
			tree->root = NULL;
		}
	} else if ( cur->r && ! cur->l ) {
		/* there's a right node, but not a left.  "replace" the
		   current node in the tree with its right child */
		if ( cur->parent->l == cur )
			cur->parent->l = cur->r;
		else if ( cur->parent->r == cur )
			cur->parent->r = cur->r;
		cur->r->parent = cur->parent;
	} else if ( cur->l && ! cur->r ) {
		/* there's a left node, but not a right.  "replace" the
		   current node in the tree with its left child */
		if ( cur->parent->l == cur )
			cur->parent->l = cur->l;
		else if ( cur->parent->r == cur ) 
			cur->parent->r = cur->l;
		cur->l->parent = cur->parent;
	} else {
		/** @todo add a call to bst_rotate_right() or bst_rotate_left() here after
		  * rotation has been fixed.
		  */

		/* find the smallest node greater than the current one and put
		   it in the place of the current node
		 */
		bst_node_t *nextgreatest = cur->r;
		while ( nextgreatest->l )	/* search until there's no left child */
			nextgreatest = nextgreatest->l;

		/* if this has a right child, push it up to the parent */
		if ( nextgreatest->r )
			nextgreatest->parent->l = nextgreatest->r ;

		/* set the pointers */
		if ( nextgreatest != cur->r )
			nextgreatest->r = cur->r;
		nextgreatest->l = cur->l;
		nextgreatest->parent = cur->parent;

		if ( cur->parent ) {
			if ( cur->parent->l == cur )
				cur->parent->l = nextgreatest;
			else
				cur->parent->r = nextgreatest;
		} else {
			tree->root = nextgreatest;
		}
	}

	
	if ( tree->free )		/* free the data */
		tree->free( cur->data );
	free( cur );			/* free the node */

}

/* find a node containing the specified data */
bst_node_t * bst_find( bstree_t *tree, void *data ){
	bst_node_t *cur;
	int cmp;
	for ( cur = tree->root; cur; ) {
		cmp = tree->cmp( data, cur->data ); 
		if ( cmp > 0 )
			cur = cur->r;
		else if ( cmp < 0 )
			cur = cur->l;
		else
			break;
	}
	return cur;
}



void bst_rotate_right( bstree_t *tree, bst_node_t *node ){
	bst_node_t *right, *grandchild;

	right = node->r;

	if ( right->balance == rightheavy ) {
#ifdef DEBUG
		printf("rotating right-right\n");
#endif
		/* right-right rotation */
		node->r = right->l;		/* make the node's right child the one to the left */
		right->l = node;		/* move the current node down */

		right->parent = node->parent;	/* move the right node up */
		if ( node->parent ) {
			if ( node->parent->r == node )
				node->parent->r = right;
			else
				node->parent->l = right;
		} else {
			tree->root = right;
		}

		node->parent = right;

		node->balance = balanced;
		right->balance = balanced;
	} else {
		/** @bug in the right-left rotation, grandchild may be NULL, causing a segfault.
		  * likely due to the circumstances in which this type of rotation is performed. */

#ifdef DEBUG
		printf("rotating right-left\n");
#endif
		/* right-left rotation */
		grandchild = right->l;		/* replace left's right child with the grandchild */
		right->l = grandchild->r;
		if ( right->l )
			right->l->parent = right;
		grandchild->r = right;
		right->parent = grandchild;	/* replace node with grandchild, moving node down to the right */
		node->r = grandchild->l;

		grandchild->l = node;		/* move node down */
		node->parent = grandchild;

		switch ( grandchild->balance ) {
			case leftheavy:
				node->balance = balanced;
				right->balance = rightheavy;
				break;
			case balanced:
				node->balance = balanced;
				right->balance = balanced;
				break;
			case rightheavy:
				node->balance = leftheavy;
				right->balance = balanced;
				break;
		}
		grandchild->balance = balanced;
	}
}

void bst_rotate_left( bstree_t *tree, bst_node_t *node ){
	bst_node_t *left, *grandchild;

	left = node->l;

	if ( left->balance == leftheavy ) {
#ifdef DEBUG
		printf("rotating left-left\n");
#endif
		/* left-left rotation */
		node->l = left->r;	/* make the node's left child the one to the right */
		left->r = node;		/* move the current node down */

		left->parent = node->parent;	/* move the left node up */
		if ( node->parent ) {
			if ( node->parent->r == node )
				node->parent->r = left;
			else
				node->parent->l = left;
		} else {
			tree->root = left;
		}

		node->parent = left;
		node->balance = balanced;
		left->balance = balanced;
	} else {
		/** @bug in the left-right rotation, grandchild may be NULL, causing a segfault.
		  * likely due to the circumstances in which this type of rotation is performed. */

#ifdef DEBUG
		printf("rotating left-right\n");
#endif
		/* left-right rotation */
		grandchild = left->r;		/* replace left's right child with the grandchild */
		left->r = grandchild->l;
		if ( left->r )
			left->r->parent = left;
		grandchild->l = left;
		left->parent = grandchild;	/* replace node with grandchild, moving node down to the right */
		node->l = grandchild->r;

		grandchild->r = node;		/* move node down */
		node->parent = grandchild;

		switch ( grandchild->balance ) {
			case leftheavy:
				node->balance = rightheavy;
				left->balance = balanced;
				break;
			case balanced:
				node->balance = balanced;
				left->balance = balanced;
				break;
			case rightheavy:
				node->balance = balanced;
				left->balance = leftheavy;
				break;
		}
		grandchild->balance = balanced;
	}
}

/* just a wrapper that pulls out the root node & passes it to the 
   subtree traversal function.
 */
void bst_call_for_each( bstree_t *tree, void (* func)( void * ), traversal_order_t order ){
	bstn_call_for_each( tree->root, func, order );
}

void bstn_call_for_each( bst_node_t *node, void (* func)( void * ), traversal_order_t order ){
	switch ( order ) {
		case preorder:
			bstn_call_preorder(node, func);
			break;
		case inorder:
			bstn_call_inorder(node, func);
			break;
		case postorder:
			bstn_call_postorder(node, func);
			break;
		case breadthfirst:
			bstn_call_breadthfirst(node, func);
			break;
		default:	/* this shouldn't happen */
			return;
	}
}

void bstn_call_preorder( bst_node_t *node, void (* func)(void *) ){
	if ( ! node )
		return;
	func(node->data);
	bstn_call_preorder(node->l, func);
	bstn_call_preorder(node->r, func);
}

void bstn_call_inorder( bst_node_t *node, void (* func)(void *) ){
	if ( ! node )
		return;
	bstn_call_inorder(node->l, func);
	func(node->data);
	bstn_call_inorder(node->r, func);
}

void bstn_call_postorder( bst_node_t *node, void (* func)(void *) ){
	if ( ! node )
		return;
	bstn_call_postorder(node->l, func);
	bstn_call_postorder(node->r, func);
	func(node->data);
}

void bstn_call_breadthfirst ( bst_node_t *node, void (* func)(void *) ){
	queue_t q;
	bst_node_t *n;

	q_init(&q, NULL);
	q_enqueue(&q, node);

	while ( ! q_empty(&q) ) {
		n = (bst_node_t *) q_dequeue(&q);
		func(n->data);
		if ( n->l )
			q_enqueue(&q, n->l);
		if ( n->r )
			q_enqueue(&q, n->r);
	}

	q_destroy(&q);
}


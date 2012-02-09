#include <stdlib.h>
#include <string.h>
#include <crush/bstree.h>
#include "unittest.h"

typedef enum {BALANCED, LEFT_HEAVY, RIGHT_HEAVY} test_balanced_t;

bstree_t * make_tree(test_balanced_t kind, int do_init_asserts) {
  bstree_t *tree = malloc(sizeof(bstree_t));
  bst_node_t *node;
  bst_init(tree, strcmp, free);
  if (do_init_asserts) {
    ASSERT_TRUE(tree->root == NULL, "bts_init: set root to NULL");
    ASSERT_TRUE(tree->cmp == strcmp, "bts_init: set compare fn");
    ASSERT_TRUE(tree->free == free, "bts_init: set free fn");
  }
  if (kind == BALANCED) {
    node = bst_insert(tree, strdup("BBB"));
    node = bst_insert(tree, strdup("AAA"));
    node = bst_insert(tree, strdup("CCC"));
  } else if (kind == LEFT_HEAVY) {
    node = bst_insert(tree, strdup("CCC"));
    node = bst_insert(tree, strdup("BBB"));
    node = bst_insert(tree, strdup("AAA"));
  } else if (kind == RIGHT_HEAVY) {
    node = bst_insert(tree, strdup("AAA"));
    node = bst_insert(tree, strdup("BBB"));
    node = bst_insert(tree, strdup("CCC"));
  }
  return tree;
}

void test_bst(test_balanced_t kind) {
  bstree_t *tree;
  bst_node_t *node;

  if (kind == BALANCED)
    fprintf(stderr, "Testing balanced tree\n");
  else if (kind == LEFT_HEAVY)
    fprintf(stderr, "Testing left-heavy tree\n");
  else if (kind == RIGHT_HEAVY)
    fprintf(stderr, "Testing right-heavy tree\n");

  tree = make_tree(kind, 0);
  ASSERT_TRUE(bst_find(tree, "AAA") != NULL,
              "bst_find: look up existing value (0)");
  ASSERT_TRUE(bst_find(tree, "BBB") != NULL,
              "bst_find: look up existing value (1)");
  ASSERT_TRUE(bst_find(tree, "CCC") != NULL,
              "bst_find: look up existing value (2)");
  ASSERT_TRUE(bst_find(tree, "DDD") == NULL,
              "bst_find: look up non-existing value");
  node = bst_insert(tree, strdup("DDD"));
  ASSERT_TRUE(bst_find(tree, "DDD") != NULL, "bst_insert: insert an element.");

  /* This should exercise each path when run for all of the balance kinds. */
  bst_delete(tree, "AAA");
  ASSERT_TRUE(bst_find(tree, "AAA") == NULL, "bst_delete: remove element A");
  bst_delete(tree, "BBB");
  ASSERT_TRUE(bst_find(tree, "BBB") == NULL, "bst_delete: remove element B");
  bst_delete(tree, "CCC");
  ASSERT_TRUE(bst_find(tree, "CCC") == NULL, "bst_delete: remove element C");
  bst_delete(tree, "DDD");
  ASSERT_TRUE(bst_find(tree, "DDD") == NULL, "bst_delete: remove element D");

  bst_destroy(tree);
  ASSERT_TRUE(tree->root == NULL, "bst_destroy: null out structure.");
  free(tree);
}

int main (int argc, char *argv[]) {
  bstree_t *tree;
  tree = make_tree(BALANCED, 1);
  bst_destroy(tree);
  free(tree);

  test_bst(BALANCED);
  test_bst(LEFT_HEAVY);
  test_bst(RIGHT_HEAVY);

  return unittest_has_error;
}

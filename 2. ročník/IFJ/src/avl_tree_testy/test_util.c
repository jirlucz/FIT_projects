#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *subtree_prefix = "  |";
const char *space_prefix = "   ";

char *make_prefix(char *prefix, const char *suffix) {
  char *result = (char *)malloc(strlen(prefix) + strlen(suffix) + 1);
  strcpy(result, prefix);
  result = strcat(result, suffix);
  return result;
}

void avl_print_node(avl_node_t *node) {
  printf("[%s,%d]", node->key, node->data.type);
}

void avl_print_subtree(avl_node_t *tree, char *prefix, direction_t from) {
  if (tree != NULL) {
    char *current_subtree_prefix = make_prefix(prefix, subtree_prefix);
    char *current_space_prefix = make_prefix(prefix, space_prefix);

    if (from == left) {
      printf("%s\n", current_subtree_prefix);
    }

    avl_print_subtree(
        tree->right,
        from == left ? current_subtree_prefix : current_space_prefix, right);

    printf("%s  +-", prefix);
    avl_print_node(tree);
    printf("\n");

    avl_print_subtree(
        tree->left,
        from == right ? current_subtree_prefix : current_space_prefix, left);

    if (from == right) {
      printf("%s\n", current_subtree_prefix);
    }

    free(current_space_prefix);
    free(current_subtree_prefix);
  }
}

void avl_print_tree(avl_node_t *tree) {
  printf("Binary tree structure:\n");
  printf("\n");
  if (tree != NULL) {
    avl_print_subtree(tree, "", none);
  } else {
    printf("Tree is empty\n");
  }
  printf("\n");
}

void avl_insert_many(avl_node_t **tree, char* keys[], avl_Data data, int count) {
  for (int i = 0; i < count; i++) {
    *tree = avl_insert(*tree, keys[i], data);
  }
}

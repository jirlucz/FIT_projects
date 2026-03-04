#ifndef BTREE_TEST_UTIL_H
#define BTREE_TEST_UTIL_H

#include "../symtable.h"
#include <stdio.h>

#define TEST(NAME, DESCRIPTION)                                                \
  void NAME() {                                                                \
    printf("[%s] %s\n", #NAME, DESCRIPTION);                                   \
    avl_node_t *test_tree;                                                     \

#define ENDTEST                                                                \
  printf("\n");                                                                \
  avl_dispose(&test_tree);                                                     \
  }

typedef enum direction { left, right, none } direction_t;

void avl_print_subtree(avl_node_t *tree, char *prefix, direction_t from);
void avl_print_tree(avl_node_t *tree);
void avl_insert_many(avl_node_t **tree, char* keys[], avl_Data data,
                     int count);
#endif

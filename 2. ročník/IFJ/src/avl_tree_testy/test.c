#include "../symtable.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>

#define NEW_SCOPE(LocalST, max_scope, current_scope, top) do               \
  {avl_init(&LocalST[++max_scope]);                                         \
  current_scope[++top] = max_scope;                                          \
  }while(0);                                                                  \

#define END_OF_SCOPE(top) do                                                \
  {top--;                                                                    \
  }while(0);                                                                  \

// result - kontrola redefinice proměnné nebo funkce
#define INSERT_SYMBOL(LocalST, index, identifier, symTableData) do                                                                   \
  {avl_Data *result = avl_search(LocalST[index], identifier);                        \
  if (result != NULL)                                                                                     \
      return;                                                                               \
  LocalST[index] = avl_insert(LocalST[index], identifier, symTableData);     \
  }while(0);                                                                                              \

#define MAX_ARGS 1000

#define MAX_TSYM 1000

#define STACK_SIZE 1000

typedef struct
{
    avl_node_t **LocalST;            // Pole všech tabulek symbolů, globální tabulka symbolů je na indexu 0
    avl_Data symTableData;
    int current_scope[MAX_TSYM];     // Pomocné pole pro uchovávání indexů rozsahů
    int top;                         // Index vrcholu pole
    int max_scope;                   // Celkový počet rozsahů
    // Flagy:
    bool Return_type;                // Funkce má návrotový typ
    bool type_assigned;              // Proměnná už má přiřazený datový typ
} ParserDataT;

void InitParserData(ParserDataT *Data)
{
  Data->LocalST = (avl_node_t **)malloc(sizeof(avl_node_t *) * MAX_TSYM);
  avl_init(&Data->LocalST[0]);
  SymTableFillFuns(&Data->LocalST[0]);
  Data->current_scope[0] = 0;
  Data->top = 0;
  Data->max_scope = 0;
  Data->symTableData.params = (char*)calloc(MAX_ARGS, sizeof(char));
  Data->symTableData.defined = false;
  Data->Return_type = false;
}

void DisposeParserData(ParserDataT *Data)
{
  for (int i = 0; i <= Data->max_scope; i++)
  {
      avl_dispose(&Data->LocalST[i]);
  }
  free(Data->LocalST);
  free(Data->symTableData.params);
}


const int base_data_count = 15;
char* base_keys[] = {"a", "b", "c", "d", "e", "f", "hh", "bs", "gfhs", "qw", "hdf", "er", "mn", "ewas", "sdb"};
avl_Data base_Value = {1, false, NULL, "id", false, false};

const int base_data_count2 = 25;
char* base_keys2[] = {"awe", "bbb", "csad", "dhfgj", "eyui", "adfg", "bvcb", "cdf", "ds", "ewq", "abg", "bas", "cnh", "drt", "edf", "asd", "bbv", "cfdgh", "dwea", "ezx", "asdf", "byrt", "cytu", "der", "egf",};


void init_test() {
  printf("Binary Search Tree - testing script\n");
  printf("-----------------------------------\n");
  printf("\n");
}

TEST(test_tree_init, "Initialize the tree")
avl_init(&test_tree);
if (test_tree == NULL)
{
  printf("\n[test_tree_init]: PASSED\n");
}
else
{
  printf("\n[test_tree_init]: FAILED\n");
}
ENDTEST

TEST(test_tree_dispose_empty, "Dispose the tree")
avl_init(&test_tree);
avl_dispose(&test_tree);
if (test_tree == NULL)
{
  printf("\n[test_tree_dispose_empty]: PASSED\n");
}
else
{
  printf("\n[test_tree_dispose_empty]: FAILED\n");
}
ENDTEST

TEST(test_tree_search_empty, "Search in an empty tree (A)")
avl_init(&test_tree);
avl_Data *result = avl_search(test_tree, "a");
if (result == NULL)
{
  printf("\n[test_tree_search_empty]: PASSED\n");
}
else
{
  printf("\n[test_tree_search_empty]: FAILED\n");
}

ENDTEST

TEST(test_tree_insert_root, "Insert an item")
avl_init(&test_tree);
test_tree = avl_insert(test_tree, "a", base_Value);
avl_print_tree(test_tree);
if (test_tree->key[0] == 'a')
{
  printf("\n[test_tree_insert_root]: PASSED\n");
}
else
{
  printf("\n[test_tree_insert_root]: FAILED\n");
}
ENDTEST

TEST(test_tree_search_root, "Search in a single node tree")
avl_init(&test_tree);
test_tree = avl_insert(test_tree, "b", base_Value);
avl_Data *result = avl_search(test_tree, "b");
avl_print_tree(test_tree);
if (result == NULL)
{
  printf("\n[test_tree_search_root]: FAILED\n");
}
else
{
  printf("\n[test_tree_search_root]: PASSED\n");
}
ENDTEST

TEST(test_tree_insert_many, "Insert many values")
avl_init(&test_tree);
avl_insert_many(&test_tree, base_keys, base_Value, base_data_count);
avl_print_tree(test_tree);
ENDTEST

TEST(test_tree_insert_many2, "Insert more values")
avl_init(&test_tree);
avl_insert_many(&test_tree, base_keys2, base_Value, base_data_count2);
avl_print_tree(test_tree);
ENDTEST

TEST(test_tree_search, "Search for an item in the tree")
avl_init(&test_tree);
avl_insert_many(&test_tree, base_keys, base_Value, base_data_count);
avl_Data *result = avl_search(test_tree, base_keys[1]);
avl_print_tree(test_tree);
if (result == NULL)
{
  printf("\n[test_tree_search]: FAILED\n");
}
else
{
  printf("\n[test_tree_search]: PASSED\n");
}
ENDTEST

TEST(test_tree_search_missing, "Search for a missing key")
avl_init(&test_tree);
avl_insert_many(&test_tree, base_keys, base_Value, base_data_count);
avl_Data *result = avl_search(test_tree, "qqq");
avl_print_tree(test_tree);
if (result == NULL)
{
  printf("\n[test_tree_search]: PASSED\n");
}
else
{
  printf("\n[test_tree_search]: FAILED\n");
}
ENDTEST

TEST(test_tree_insert_functions, "Insert predefined functions")
avl_init(&test_tree);
SymTableFillFuns(&test_tree);
avl_print_tree(test_tree);
ENDTEST

TEST(test_tree_search_function, "Search for a predefined function")
avl_init(&test_tree);
SymTableFillFuns(&test_tree);
avl_Data *result = avl_search(test_tree, "readDouble");
avl_print_tree(test_tree);
if (result == NULL)
{
  printf("\n[test_tree_search]: FAILED\n");
}
else if(strcmp(result->identifier, "readDouble") == 0)
{
  printf("\n[test_tree_search]: PASSED\n");
}
ENDTEST

TEST(test_tree_dispose_filled, "Dispose the whole tree")
avl_init(&test_tree);
avl_insert_many(&test_tree, base_keys, base_Value, base_data_count);
avl_print_tree(test_tree);
avl_dispose(&test_tree);
avl_print_tree(test_tree);
if (test_tree == NULL)
{
  printf("\n[test_tree_dispose_filled]: PASSED\n");
}
else
{
  printf("\n[test_tree_dispose_filled]: FAILED\n");
}
ENDTEST

TEST(test_multiple_symtables, "Create structure for symbol tables")
avl_init(&test_tree);
ParserDataT gData;
InitParserData(&gData);
gData.symTableData = base_Value;

for (int i = 0; i < 10; i++)
{
  strcpy(gData.symTableData.identifier, base_keys[i]);
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  if ((i == 2) || (i == 5))
  {
    NEW_SCOPE(gData.LocalST, gData.max_scope, gData.current_scope, gData.top);
  }
  if ((i == 6) || (i == 8))
  {
    END_OF_SCOPE(gData.top);
  }
}
printf("počet rozsahů = %d\n", gData.max_scope);
for (int i = 0; i <= gData.max_scope; i++)
{
printf("rozsah č. %d\n", i);
  avl_print_tree(gData.LocalST[i]);
}

DisposeParserData(&gData);
ENDTEST


int main(int argc, char *argv[]) {
  init_test();

  test_tree_init();
  test_tree_dispose_empty();
  test_tree_search_empty();
  test_tree_insert_root();
  test_tree_search_root();
  test_tree_insert_many();
  test_tree_insert_many2();
  test_tree_search();
  test_tree_search_missing();
  test_tree_insert_functions();
  test_tree_search_function();
  test_tree_dispose_filled();
  test_multiple_symtables();
}
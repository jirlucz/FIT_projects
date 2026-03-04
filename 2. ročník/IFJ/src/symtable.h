/*
 * @file symtable.h
 *
 * Hlavičkový soubor pro strukturu AVL stromu.
 * 
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ID_MAX_LEN 10000
#define MAX_PARAMS 1000

typedef enum
{
	TYPE_FUNCTION,		// Not a variable but a function
	TYPE_INT,			// Integer data type
	TYPE_DOUBLE,		// Double data type
	TYPE_STRING,		// String data type
	TYPE_UNDEFINED		// Type not defined
} Data_type;

typedef struct
{
	Data_type type;								// Data type of symbol / return type of function
	bool defined;								// Defined if current function or variable was defined
	char params[MAX_PARAMS];           					// parameters in string form
	char identifier[ID_MAX_LEN];				// Data identifier (key)
	bool initialized;							// Determines whether the value of a variable is initialized
	bool optional;								// Variable declared with '?'
	bool constant;								// Variable declared with 'let'
	bool underscore;							// Variable defined with '_'
	bool underscore_param[MAX_PARAMS];			// Function parameters defined with '_'
	char *param_ids[MAX_PARAMS];		// Identificators of functions parameters
} avl_Data;

// Uzel stromu
typedef struct avl_node {
    char* key;
    avl_Data data;
    struct avl_node *left;  // levý potomek
    struct avl_node *right; // pravý potomek
    int height;
} avl_node_t;


void avl_init(avl_node_t **tree);

int avl_height(avl_node_t *N);

int max(int a, int b);

avl_node_t* newNode(char* key, avl_Data data);

avl_node_t* avl_rightRotate(avl_node_t *y);

avl_node_t* avl_leftRotate(avl_node_t *x);

int getBalance(avl_node_t *N);

avl_node_t *avl_insert(avl_node_t *node, char* key, avl_Data data);

avl_Data *avl_search(avl_node_t *tree, char* key);

void avl_dispose(avl_node_t **tree);

void SymTableFillFuns(avl_node_t **tree);

#endif

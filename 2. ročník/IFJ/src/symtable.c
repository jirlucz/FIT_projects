/*
 * @file symtable.c
 *
 * @author David Nepraš
 * 
 * @brief Implementace Výškově vyváženého binárního stromu jako tabulku symbolů
 */

#include "symtable.h"

void avl_init(avl_node_t **tree)
{
    *tree = NULL;
}

int avl_height(avl_node_t *N)
{
    if (N == NULL) 
        return 0; 
    return N->height; 
}

int max(int a, int b) 
{ 
    return (a > b)? a : b; 
} 

avl_node_t* newNode(char* key, avl_Data data) 
{ 
    avl_node_t* node = (avl_node_t*)malloc(sizeof(avl_node_t)); 
    node->key = (char*)calloc(strlen(key) + 1, sizeof(char));
    strcpy(node->key, key); // node->key = key;
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return(node);
} 

avl_node_t* avl_rightRotate(avl_node_t *y)
{
    avl_node_t *x = y->left; 
    avl_node_t *T2 = x->right; 
  
    x->right = y; 
    y->left = T2; 
  
    y->height = max(avl_height(y->left), avl_height(y->right)) + 1; 
    x->height = max(avl_height(x->left), avl_height(x->right)) + 1; 
  
    return x;
}

avl_node_t* avl_leftRotate(avl_node_t *x)
{
    avl_node_t *y = x->right; 
    avl_node_t *T2 = y->left; 
  
    y->left = x; 
    x->right = T2; 
  
    x->height = max(avl_height(x->left), avl_height(x->right)) + 1; 
    y->height = max(avl_height(y->left), avl_height(y->right)) + 1; 
  
    return y;
}

int getBalance(avl_node_t *N)
{ 
    if (N == NULL) 
        return 0; 
    return avl_height(N->left) - avl_height(N->right); 
}

avl_node_t *avl_insert(avl_node_t *node, char* key, avl_Data data)
{
    if (node == NULL) 
        return(newNode(key, data)); 
  
    int result = strcmp(key, node->key);
    if (result < 0)  // key < node->key
        node->left  = avl_insert(node->left, key, data);
    else if (result > 0)   // key > node->key
        node->right = avl_insert(node->right, key, data);
    else // Equal keys are not allowed in BST
        return node;
  
    node->height = 1 + max(avl_height(node->left), 
                        avl_height(node->right)); 
  
    int balance = getBalance(node); 
  
  if (node->left != NULL) {
    result = strcmp(key, node->left->key);
    if (balance > 1 && result < 0)  // key < node->left->key
        return avl_rightRotate(node); 
  }
  if (node->right != NULL) {
    result = strcmp(key, node->right->key); 
    if (balance < -1 && result > 0) // key > node->right->key
        return avl_leftRotate(node); 
  }
  if (node->left != NULL) {
    result = strcmp(key, node->left->key);
    if (balance > 1 && result > 0) // key > node->left->key
    { 
        node->left =  avl_leftRotate(node->left); 
        return avl_rightRotate(node); 
    } 
  }
  if (node->right != NULL) {
    result = strcmp(key, node->right->key);
    if (balance < -1 && result < 0) // key < node->right->key
    { 
        node->right = avl_rightRotate(node->right);
        return avl_leftRotate(node); 
    } 
  }
    /* return the (unchanged) node pointer */
    return node;
}

avl_Data *avl_search(avl_node_t *tree, char* key)
{
      // Pokud jsme došli na konec stromu, klíč nebyl nalezen.
  if (tree == NULL) {
    return NULL;
  }
  int result = strcmp(key, tree->key);
  // Pokud klíč našeho uzlu odpovídá hledanému klíči, uložíme hodnotu a vrátíme true.
  if (result == 0) {
    return &tree->data;
  }
  // Pokud hledaný klíč je menší než klíč aktuálního uzlu, pokračujeme ve vyhledávání v levém podstromu.
  else if (result < 0) {
    return avl_search(tree->left, key);
  }
  // Jinak v pravém.
  else {
    return avl_search(tree->right, key);
  }

}

void avl_dispose(avl_node_t **tree)
{
    // Pokud jsme došli na konec stromu (prázdný podstrom), končíme rekurzi.
    if (*tree == NULL) {
    return;
    }

    // Rekurzivně uvolníme levý a pravý podstrom.
    avl_dispose(&(*tree)->left);
    avl_dispose(&(*tree)->right);

    // Nakonec uvolníme aktuální uzel a nastavíme ukazatel na NULL.
    free(*tree);
    *tree = NULL;
}

/**
 * Fills given symbol table with the predefined functions
 * 
 */ 
void SymTableFillFuns (avl_node_t **tree)
{
    avl_Data newData;
    newData.type = TYPE_FUNCTION;
    newData.defined = true;

    newData.params[0] = 's';
    newData.params[1] = '\0';
    strcpy(newData.identifier, "readString");
    newData.optional = true;
    *tree = avl_insert(*tree, "readString", newData);

    newData.params[0] = 'i';
    newData.params[1] = '\0';
    strcpy(newData.identifier, "readInt");
    newData.optional = true;
    *tree = avl_insert(*tree, "readInt", newData);

    newData.params[0] = 'd';
    newData.params[1] = '\0';
    strcpy(newData.identifier, "readDouble");
    newData.optional = true;
    *tree = avl_insert(*tree, "readDouble", newData);

    newData.params[0] = 'v';
    newData.params[1] = 'x';      // x = proměnný počet parametrů
    newData.params[2] = '\0';
    strcpy(newData.identifier, "write");
    *tree = avl_insert(*tree, "write", newData);

    newData.params[0] = 'd';
    newData.params[1] = 'i';
    newData.params[2] = '\0';
    strcpy(newData.identifier, "Int2Double");
    newData.underscore_param[0] = true;
    newData.optional = false;
    *tree = avl_insert(*tree, "Int2Double", newData);

    newData.params[0] = 'i';
    newData.params[1] = 'd';
    newData.params[2] = '\0';
    strcpy(newData.identifier, "Double2Int");
    newData.underscore_param[0] = true;
    newData.optional = false;
    *tree = avl_insert(*tree, "Double2Int", newData);

    newData.params[0] = 'i';
    newData.params[1] = 's';
    newData.params[2] = '\0';
    strcpy(newData.identifier, "length");
    newData.underscore_param[0] = true;
    newData.optional = false;
    *tree = avl_insert(*tree, "length", newData);

    newData.params[0] = 's';
    newData.params[1] = 's';
    newData.params[2] = 'i';
    newData.params[3] = 'i';
    newData.params[4] = '\0';
    strcpy(newData.identifier, "substring");
    newData.underscore_param[0] = false;
    newData.underscore_param[1] = false;
    newData.underscore_param[2] = false;
    newData.param_ids[0] = "of";
    newData.param_ids[1] = "startingAt";
    newData.param_ids[2] = "endingBefore";
    newData.optional = true;
    *tree = avl_insert(*tree, "substring", newData);

    newData.params[0] = 'i';
    newData.params[1] = 's';
    newData.params[2] = '\0';
    strcpy(newData.identifier, "ord");
    newData.underscore_param[0] = true;
    newData.optional = false;
    *tree = avl_insert(*tree, "ord", newData);

    newData.params[0] = 's';
    newData.params[1] = 'i';
    newData.params[2] = '\0';
    strcpy(newData.identifier, "chr");
    newData.underscore_param[0] = true;
    newData.optional = false;
    *tree = avl_insert(*tree, "chr", newData);
}

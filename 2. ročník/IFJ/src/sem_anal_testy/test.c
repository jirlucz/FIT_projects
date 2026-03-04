#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>

#define LEX_ERR 1               // lexical analysis error
#define SYN_ERR 2               // syntax analysis error
#define SEM_UF_ERR 3            // undefined function, redefinition of variable
#define SEM_VAR_CNT_ERR 4       // incorrect function parameters or wrong return type of function
#define SEM_UNDEF_VAR_ERR 5     // usage of an undefined or uninitialized variable
#define SEM_FUN_RETURN_ERR 6    // missing or remaining in return command of function
#define SEM_TYPES_AR_ERR 7      // type compatibility in aritmetic, string or relational expressions
#define SEM_TYPES_UNSPEC_ERR 8  // unspecified type of variable
#define SEM_OTHER_ERR 9         // other semantic errors
#define INTERNAL_ERR 99         // internal failure of compiler

#define LEX_OK 11               // lexical analysis success
#define SYN_OK 12               // syntax analysis success
#define STACK_OK 13             // stack success


#define NEW_TOKEN() do                                                       \
    {token_first = token_prev;                                               \
    token_prev = gData->token;                                               \
    gData->token = &tokens[i++];                                                          \
    } while (0);                                                             \

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

Token tokens1[] = { {0, NULL, 0, 0, VAR, false, false}, {1, "id1", 0, 0, IDENTIFICATOR, false, false}, {2, NULL, 0, 0, DOUBLEDOT, false, false},
                 {3, NULL, 0, 0, INTKEY, false, false}, {4, NULL, 0, 0, EQ, false, false}, {5, NULL, 3, 3, WHOLENUMBER, true, false}, {6, NULL, 0, 0, EOL, false, false},
                 {7, NULL, 0, 0, LET, false, false}, {8, "id2", 0, 0, IDENTIFICATOR, false, false}, {9, NULL, 0, 0, DOUBLEDOT, false, false},
                 {10, NULL, 0, 0, INTKEY, false, false}, {11, NULL, 0, 0, EQ, false, false}, {12, NULL, 4, 4, WHOLENUMBER, true, false}, {13, NULL, 0, 0, EOL, false, false},
                 {14, NULL, 0, 0, IF, false, false}, {15, NULL, 0, 0, OPBR, false, false}, {16, "id1", 0, 0, IDENTIFICATOR, false, false},
                 {17, NULL, 0, 0, LESSER, false, false}, {18, "id2", 0, 0, IDENTIFICATOR, false, false}, {19, NULL, 0, 0, CLBR, false, false},
                 {20, NULL, 0, 0, OPCB, true, false}, {21, NULL, 0, 0, EOL, false, false}, {22, NULL, 0, 0, VAR, false, false}, {23, "id3", 0, 0, IDENTIFICATOR, false, false},
                 {24, NULL, 0, 0, DOUBLEDOT, false, false}, {25, NULL, 0, 0, DOUBLEKEY, false, false}, {26, NULL, 0, 0, EQ, false, false},
                 {27, NULL, 3, 3, WHOLENUMBER, true, false}, {28, NULL, 0, 0, EOL, false, false}, {29, NULL, 0, 0, CLCB, true, false}, {30, NULL, 0, 0, EOL, false, false}, {31, NULL, 4, 4, ELSE, false, false},
                 {32, NULL, 0, 0, OPCB, true, false}, {33, NULL, 0, 0, EOL, false, false}, {34, NULL, 0, 0, VAR, false, false}, {35, "id4", 0, 0, IDENTIFICATOR, false, false},
                 {36, NULL, 0, 0, DOUBLEDOT, false, false}, {37, NULL, 0, 0, DOUBLEKEY, false, false}, {38, NULL, 4, 4, EQ, false, false},
                 {39, NULL, 3, 3, WHOLENUMBER, true, false}, {40, NULL, 0, 0, EOL, false, false}, {41, NULL, 0, 0, CLCB, true, true}, {42, NULL, 0, 0, EOFILE, true, true}};

avl_Data base_Values1[] = {{TYPE_INT, false, NULL, "id1", true, false, false}, {TYPE_INT, false, NULL, "id2", true, false, true},
                           {TYPE_DOUBLE, false, NULL, "id3", true, false, false}, {TYPE_DOUBLE, false, NULL, "id4", true, false, false}};


Token tokens2[] = { {0, NULL, 0, 0, VAR, false, false}, {1, "id1", 0, 0, IDENTIFICATOR, false, false}, {2, NULL, 0, 0, DOUBLEDOT, false, false},
                 {3, NULL, 0, 0, INTKEY, false, false}, {4, NULL, 0, 0, EQ, false, false}, {5, NULL, 3, 3, WHOLENUMBER, true, false}, {6, NULL, 0, 0, EOL, false, false},
                 {7, NULL, 0, 0, VAR, false, false}, {8, "id2", 0, 0, IDENTIFICATOR, false, false}, {9, NULL, 0, 0, DOUBLEDOT, false, false},
                 {10, NULL, 0, 0, INTKEY, false, false}, {11, NULL, 0, 0, EQ, false, false}, {8, "id3", 0, 0, IDENTIFICATOR, true, false}, {13, NULL, 0, 0, EOL, false, true},
                 {42, NULL, 0, 0, EOFILE, true, true}};

avl_Data base_Values2[] = {{TYPE_INT, false, NULL, "id1", true, false, false}, {TYPE_INT, false, NULL, "id2", true, false, true}};


Token tokens3[] = { {0, NULL, 0, 0, VAR, false, false}, {1, "id1", 0, 0, IDENTIFICATOR, false, false}, {2, NULL, 0, 0, DOUBLEDOT, false, false},
                 {3, NULL, 0, 0, INTKEY, false, false}, {4, NULL, 0, 0, EQ, false, false}, {5, NULL, 3, 3, WHOLENUMBER, true, false}, {6, NULL, 0, 0, EOL, false, false},
                 {7, NULL, 0, 0, VAR, false, false}, {8, "id2", 0, 0, IDENTIFICATOR, false, false}, {9, NULL, 0, 0, DOUBLEDOT, false, false},
                 {10, NULL, 0, 0, INTKEY, true, false}, {11, NULL, 0, 0, EOL, false, false}, {12, NULL, 0, 0, VAR, false, false}, {13, "id3", 0, 0, IDENTIFICATOR, true, false},
                 {14, NULL, 0, 0, DOUBLEDOT, false, false}, {15, NULL, 0, 0, INTKEY, false, false}, {16, NULL, 0, 0, EQ, true, true},
                 {17, "id2", 0, 0, IDENTIFICATOR, true, false}, {18, NULL, 0, 0, EOL, false, true}, {42, NULL, 0, 0, EOFILE, true, true}};

avl_Data base_Values3[] = {{TYPE_INT, false, NULL, "id1", true, false, false}, {TYPE_INT, false, NULL, "id2", false, false, false},
                           {TYPE_INT, false, NULL, "id3", true, false, false}};


Token tokens4[] = { {0, NULL, 0, 0, FUNC, false, false}, {1, "foo", 0, 0, IDENTIFICATOR, false, false}, {2, NULL, 0, 0, OPBR, false, false},
                 {3, NULL, 0, 0, UNDERSCORE, false, false}, {4, "id1", 0, 0, IDENTIFICATOR, false, false}, {5, NULL, 3, 3, DOUBLEDOT, false, false}, {6, NULL, 0, 0, INTKEY, false, false},
                 {7, NULL, 0, 0, COMA, false, false}, {8, "id2", 0, 0, IDENTIFICATOR, false, false}, {9, NULL, 0, 0, DOUBLEDOT, false, false},
                 {10, NULL, 0, 0, INTKEY, false, false}, {11, NULL, 0, 0, CLBR, false, false}, {12, NULL, 0, 0, LAMBDA, false, false}, {13, NULL, 0, 0, INTKEY, false, false},
                 {14, NULL, 0, 0, OPCB, true, false}, {15, NULL, 0, 0, EOL, false, false}, {16, NULL, 0, 0, VAR, false, false},
                 {17, "id3", 0, 0, IDENTIFICATOR, false, false}, {18, NULL, 0, 0, DOUBLEDOT, false, false}, {19, NULL, 0, 0, INTKEY, false, false},
                 {20, NULL, 0, 0, EQ, false, false}, {21, "id1", 0, 0, IDENTIFICATOR, false, false}, {22, NULL, 0, 0, PLUS, false, false}, {23, "id2", 0, 0, IDENTIFICATOR, true, false},
                 {24, NULL, 0, 0, EOL, false, false}, {25, NULL, 0, 0, RETURN, false, false}, {26, "id3", 0, 0, IDENTIFICATOR, true, false},
                 {27, NULL, 3, 3, EOL, false, false}, {28, NULL, 0, 0, CLCB, true, false}, {29, NULL, 0, 0, EOL, false, false}, {30, "foo", 0, 0, IDENTIFICATOR, false, false}, {31, NULL, 4, 4, OPBR, false, false},
                 {32, NULL, 0, 0, WHOLENUMBER, false, false}, {33, NULL, 0, 0, COMA, false, false}, {34, "id2", 0, 0, IDENTIFICATOR, false, false}, {35, NULL, 0, 0, DOUBLEDOT, false, false},
                 {36, NULL, 0, 0, WHOLENUMBER, false, false}, {37, NULL, 0, 0, CLBR, true, false}, {38, NULL, 4, 4, EOL, false, true}, {39, NULL, 0, 0, EOFILE, true, true}};

avl_Data base_Values4[] = {{TYPE_FUNCTION, true, "iii", "foo", true, false, true, true, {true, false}, {"id1", "id2"}}, {TYPE_INT, true, NULL, "id1", true, false, true, true},
                           {TYPE_INT, true, NULL, "id2", true, false, true, false}, {TYPE_INT, true, NULL, "id3", true, false, false, false}};



typedef struct
{
    Token *token;

    avl_node_t **LocalST;            // Pole všech tabulek symbolů, globální tabulka symbolů je na indexu 0
    avl_Data symTableData;
    int current_scope[1000];     // Pomocné pole pro uchovávání indexů rozsahů
    int top;                         // Index vrcholu pole
    int max_scope;                   // Celkový počet rozsahů
    // Flagy:
    bool Return_type;                // Funkce má návrotový typ
    bool type_assigned;              // Proměnná už má přiřazený datový typ
    int number_params;
    bool assign;
} ParserDataT;

void new_token(ParserDataT* gData, Token *token_first, Token *token_prev, Token *tokens, int *i)
{
  token_first = token_prev;
  token_prev = gData->token;
  gData->token = &tokens[*i++];
}

void InitParserData(ParserDataT *Data)
{
  Data->LocalST = (avl_node_t **)malloc(sizeof(avl_node_t *) * 1000);
  avl_init(&Data->LocalST[0]);
  //SymTableFillFuns(&Data->LocalST[0]);
  Data->current_scope[0] = 0;
  Data->top = 0;
  Data->max_scope = 0;
  Data->symTableData.params = (char*)calloc(1000, sizeof(char));
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

avl_Data *search_symbol(ParserDataT *gData, char* key)
{
    for (int i = gData->top; i >= 0; i--)
    {
      avl_Data *result = avl_search(gData->LocalST[gData->current_scope[i]], key);
      if (result != NULL)
      {
          return result;
      }
    }
    return NULL;
}

int sem_anal(ParserDataT *gData, Token *tokens)
{
    //Return2Begg();
    int i = 0;
    gData->top = 0;
    gData->max_scope = 0;
    gData->current_scope[0] = 0;
    Token *token_first = NULL;
    Token *token_prev = NULL;
    int new_token = false;
    gData->token = &tokens[i++];
    bool scope_added = false;
    while (gData->token->type != EOFILE)
    {
        if (gData->token->type == IDENTIFICATOR)
        {
            switch (token_prev->type)
            {
            case LET:
            case VAR:
                if (token_prev->type == LET)
                {
                    if (token_first != NULL)
                    {
                        if (token_first->type == IF)   // (IF let id)
                        {
                            avl_Data *result = search_symbol(gData, gData->token->identif);
                            if (result == NULL)
                            {
                                return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                            }
                            else if (result->defined != true)
                            {
                                return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                            }
                            else if (result->optional != true)
                            {
                                return SEM_OTHER_ERR;  // Variable is not optional
                            }
                            else if (result->initialized != true)
                            {
                                return SEM_UNDEF_VAR_ERR;  // Use of uninitialized variable
                            }
                            break;
                        }
                    }
                }
                // Definition of a variable
                avl_Data *result = search_symbol(gData, gData->token->identif);
                result->defined = true;
                break;
            // Definition of a function
            case FUNC:
                avl_Data *function = search_symbol(gData, gData->token->identif);
                function->defined = true;
                // new scope
                gData->top++;
                gData->max_scope++;
                gData->current_scope[gData->top] = gData->max_scope;
                scope_added = true;
                break;
            default:
                NEW_TOKEN();
                if (gData->token->type == OPBR)  // Function call
                {
                    avl_Data *function = search_symbol(gData, token_prev->identif);
                    if (function == NULL)
                    {
                        return SEM_UF_ERR;  // Function is undefined
                    }
                    else if (function->defined == false)
                    {
                        return SEM_UF_ERR;  // Function is undefined
                    }
                    int num_params = 1;
                    NEW_TOKEN();    // '('
                    while (gData->token->type != CLBR)  // Parameters
                    {
                        if (function->params[1] == 'x')  // variable number of parameters
                        {
                            NEW_TOKEN();
                            continue;
                        }
                        if (function->underscore_param[num_params - 1] == true)
                        {
                            if (gData->token->type == IDENTIFICATOR)
                            {
                                avl_Data *param = search_symbol(gData, gData->token->identif);
                                if (param == NULL)
                                {
                                    return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                                }
                                else if (param->defined == false || param->initialized == false)
                                {
                                    return SEM_UNDEF_VAR_ERR;  // Use of undefined or uninitialized variable
                                }
                                switch (param->type)
                                {
                                case TYPE_INT:
                                    if (function->params[num_params] != 'i')
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                    break;
                                case TYPE_DOUBLE:
                                    if (function->params[num_params] != 'd')
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                    break;
                                case TYPE_STRING:
                                    if (function->params[num_params] != 's')
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                    break;
                                default:
                                    break;
                                }
                                num_params++;
                            }
                            else if (gData->token->type == WHOLENUMBER)
                            {
                                if (function->params[num_params] != 'i')
                                {
                                    return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                }
                                num_params++;
                            }
                            else if (gData->token->type == DECIMALNUMBER || gData->token->type)
                            {
                                if (function->params[num_params] != 'd')
                                {
                                    return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                }
                                num_params++;
                            }
                            else if (gData->token->type == STRINGVALUE)
                            {
                                if (function->params[num_params] != 's')
                                {
                                    return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                }
                                num_params++;
                            }
                        }
                        else
                        {
                            if (gData->token->type == IDENTIFICATOR)
                            {
                                if (function->param_ids[num_params - 1] != gData->token->identif)
                                {
                                    return SEM_VAR_CNT_ERR;
                                }
                                NEW_TOKEN();    // ':'
                                if (gData->token->type != DOUBLEDOT)
                                {
                                    return SEM_VAR_CNT_ERR;
                                }
                                NEW_TOKEN();    // identif/number/string
                                if (gData->token->type == IDENTIFICATOR)
                                {
                                    avl_Data *param = search_symbol(gData, gData->token->identif);
                                    if (param == NULL)
                                    {
                                        return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                                    }
                                    else if (param->defined == false || param->initialized == false)
                                    {
                                        return SEM_UNDEF_VAR_ERR;  // Use of undefined or uninitialized variable
                                    }
                                    switch (param->type)
                                    {
                                    case TYPE_INT:
                                        if (function->params[num_params] != 'i')
                                        {
                                            return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                        }
                                        break;
                                    case TYPE_DOUBLE:
                                        if (function->params[num_params] != 'd')
                                        {
                                            return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                        }
                                        break;
                                    case TYPE_STRING:
                                        if (function->params[num_params] != 's')
                                        {
                                            return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                        }
                                        break;
                                    default:
                                        break;
                                    }
                                }
                                else if (gData->token->type == WHOLENUMBER)
                                {
                                    if (function->params[num_params] != 'i')
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                }
                                else if (gData->token->type == DECIMALNUMBER)
                                {
                                    if (function->params[num_params] != 'd')
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                }
                                else if (gData->token->type == STRINGVALUE)
                                {
                                    if (function->params[num_params] != 's')
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                }
                                num_params++;
                            }
                        }
                        NEW_TOKEN();
                    }
                    if (function->params[1] != 'x')  // variable number of parameters
                    {
                        if (strlen(function->params) != num_params)
                        {
                            return SEM_VAR_CNT_ERR;  // Wrong number of parameters
                        }
                    }
                }
                else if (gData->token->type == EQ)  // assignment to a variable
                {
                    if (token_prev->type == IDENTIFICATOR)
                    {
                        avl_Data *result = search_symbol(gData, token_prev->identif);
                        if (result == NULL)
                        {
                            return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                        }
                        else if (result->defined == false)
                        {
                            return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                        }
                        else if (result->constant == true)
                        {
                            return SEM_OTHER_ERR;  // Can't assing to a constant
                        }
                        result->initialized = true;
                    }
                }
                else    // Use of a variable
                {
                  avl_Data *result = search_symbol(gData, token_prev->identif);
                  if (result == NULL)
                  {
                      return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                  }
                  else if (result->defined == false)
                  {
                      return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                  }
                  else if (result->initialized == false)
                  {
                      return SEM_UNDEF_VAR_ERR;  // Use of uninitialized variable
                  }
                  new_token = true;
                }
                break;
            }
        }
        if (gData->token->type == OPCB)  // new scope
        {
            if (scope_added == false)
            {
                gData->top++;
                gData->max_scope++;
                gData->current_scope[gData->top] = gData->max_scope;
            }
            else
            {
                scope_added = false;
            }
        }
        if (gData->token->type == CLCB)  // end of scope
        {
          END_OF_SCOPE(gData->top);
        }
        if (new_token == false)
        {
            NEW_TOKEN();
        }
        new_token = false;
    }
    return 0;
}

void init_test() {
  printf("Semantic analysis - testing script\n");
  printf("-----------------------------------\n");
  printf("\n");
}

TEST(Code1, "Code1 - semantically correct")
  gData.symTableData = base_Values1[0];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  
  gData.symTableData = base_Values1[1];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);

  NEW_SCOPE(gData.LocalST, gData.max_scope, gData.current_scope, gData.top);
  gData.symTableData = base_Values1[2];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  END_OF_SCOPE(gData.top);

  NEW_SCOPE(gData.LocalST, gData.max_scope, gData.current_scope, gData.top);
  gData.symTableData = base_Values1[3];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  END_OF_SCOPE(gData.top);

  int sem_result = sem_anal(&gData, tokens1);
  if (sem_result != 0)
  {
      printf("result of semantic analysis = %d\n", sem_result);
  }
  else
  {
      printf("Code 1 semantically correct\n");
  }
ENDTEST

TEST(Code2, "Code2 - undefined variable")
  gData.symTableData = base_Values2[0];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  
  gData.symTableData = base_Values2[1];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);

  int sem_result = sem_anal(&gData, tokens2);
  if (sem_result != 0)
  {
      printf("result of semantic analysis = %d\n", sem_result);
  }
  else
  {
      printf("Code 2 semantically correct\n");
  }
ENDTEST

TEST(Code3, "Code3 - uninitialized variable")
  gData.symTableData = base_Values3[0];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  
  gData.symTableData = base_Values3[1];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);

  gData.symTableData = base_Values3[2];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);

  int sem_result = sem_anal(&gData, tokens3);
  if (sem_result != 0)
  {
      printf("result of semantic analysis = %d\n", sem_result);
  }
  else
  {
      printf("Code 3 semantically correct\n");
  }
ENDTEST

TEST(Code4, "Code4 - code with function semantically correct")
  gData.symTableData = base_Values4[0];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  NEW_SCOPE(gData.LocalST, gData.max_scope, gData.current_scope, gData.top);

  gData.symTableData = base_Values4[1];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);

  gData.symTableData = base_Values4[2];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);

  gData.symTableData = base_Values4[3];
  INSERT_SYMBOL(gData.LocalST, gData.current_scope[gData.top], gData.symTableData.identifier, gData.symTableData);
  END_OF_SCOPE(gData.top);

// printf("počet rozsahů = %d\n", gData.max_scope);
// for (int i = 0; i <= gData.max_scope; i++)
// {
// printf("rozsah č. %d\n", i);
//   avl_print_tree(gData.LocalST[i]);
// }


  int sem_result = sem_anal(&gData, tokens4);
  if (sem_result != 0)
  {
      printf("result of semantic analysis = %d\n", sem_result);
  }
  else
  {
      printf("Code 4 semantically correct\n");
  }
ENDTEST

int main(int argc, char *argv[]) {
  init_test();

  Code1();
  Code2();
  Code3();
  Code4();
}
/**
 * @file parser.c
 *
 * @brief Implements syntax, semantic and code generation functions.
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 * @author Dominik Honza <xhonza04@stud.fit.vutbr.cz>
 * @author David Nepraš <xnepra02@stud.fit.vutbr.cz>
 */

#include "expr.h"
#include "parser.h"
#include "generator.h"

// Debug PRINT
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define CALL_GET_TOKEN() do                                                 \
    {if ((result = GetToken(gData->token)) != LEX_OK){                      \
        DEBUG_PRINT("%s\n", "GET TOKEN: LEX ERR");                          \
        return result;                                                      \
    }                                                                       \
        DEBUG_PRINT("GET TOKEN: %d\n", gData->token->type);                 \
    }while(0)

#define GET_TOKEN() do                                                      \
    {if (gData->pushBack) {                                                 \
        DEBUG_PRINT("GET TOKEN_P: %d\n", gData->token->type);               \
        gData->pushBack = false;}                                           \
     else                                                                   \
         CALL_GET_TOKEN();                                                  \
    }while(0)

#define GET_TOKEN_AND_EXPECT_TYPE(_type) do                                  \
    {GET_TOKEN();                                                           \
    if (gData->token->type != _type)                                         \
        return SYN_ERR;                                                     \
    }while(0)

#define EXPECT_TYPE(_type)                                                   \
    if (gData->token->type != _type)                                         \
        return SYN_ERR

#define EXEC_RULE(rule) do                                                  \
    {result = rule(gData);                                                  \
    if (result != SYN_OK)                                                   \
        return result;                                                      \
    }while(0);                                                              \

#define IS_STAT_FIRST(type)                                                 \
	(type == IDENTIFICATOR || type == IF || type == WHILE || type == VAR    \
        || type == LET)

#define IS_EXPR_FIRST(type)                                                 \
    (type == WHOLENUMBER || type == DECIMALNUMBER || type == STRINGVALUE    \
        || type == OPBR)

#define IS_VALUE(type)                                                      \
    (type == WHOLENUMBER || type == DECIMALNUMBER || type == STRINGVALUE    \
        || type == NIL)

#define NEW_SCOPE(LocalST, max_scope, current_scope, top) do                    \
    {avl_init(&LocalST[++max_scope]);                                           \
    current_scope[++top] = max_scope;                                           \
    }while(0);                                                                  \

#define END_OF_SCOPE(top) do                                                    \
    {top--;                                                                     \
    }while(0);                                                                  \

// res - kontrola redefinice proměnné nebo funkce, u parametru definovaneho s '_' redefinice povolena
#define INSERT_SYMBOL(LocalST, index, identifier, symTableData) do                                        \
  {avl_Data *res = avl_search(LocalST[index], identifier);                                                \
  if (res != NULL){                                                                                       \
    if (res->underscore == true)                                                                          \
        res->underscore = false;                                                                           \
    else                                                                                                  \
      return SEM_UF_ERR;                                                                                  \
  }                                                                                                     \
  LocalST[index] = avl_insert(LocalST[index], identifier, symTableData);                                  \
  }while(0);                                                                                              \

#define TSDATA_RESET(Data) do                                                                            \
    {Data->symTableData.defined = false;                                                                 \
    Data->symTableData.initialized = false;                                                              \
    Data->symTableData.optional = false;                                                                 \
    Data->symTableData.constant = false;                                                                 \
    Data->Return_type = false;                                                                           \
    Data->type_assigned = false;                                                                         \
    Data->symTableData.underscore = false;                                                               \
    Data->symTableData.type = TYPE_UNDEFINED;                                                            \
    gData->assign = false;                                                                                \
    } while (0);                                                                                         \
    
#define NEW_TOKEN() do                                                       \
    {token_prev = *gData->token;                                               \
    int res;                                                                 \
    if ((res = GetToken(gData->token)) != LEX_OK)                            \
        return res;                                                          \
    } while (0);                                                             \


#define MAX_ARGS 1000

#define MAX_TSYM 1000

#define STACK_SIZE 1000

int result = 0;

void InitParserData(ParserDataT *Data)
{
  Data->token = malloc(sizeof(Token));
  InitToken(Data->token);

  Data->LocalST = (avl_node_t **)malloc(sizeof(avl_node_t *) * MAX_TSYM);
  avl_init(&Data->LocalST[0]);
  SymTableFillFuns(&Data->LocalST[0]);
  Fluids*code = (Fluids*) malloc (sizeof (Fluids));
  FluidInit(code);
  Data->current_scope[0] = 0;
  Data->top = 0;
  Data->max_scope = 0;
  Data->symTableData.defined = false;
  Data->Return_type = false;
  Data->symTableData.initialized = false;
  Data->symTableData.constant = false;
  Data->type_assigned = false;
  Data->number_params = 0;
  Data->code = code;
  Data->symTableData.type = TYPE_UNDEFINED;
  Data->result_type = TYPE_UNDEFINED;
  Data->pushBack = false;
  Data->argExpr = false;
  Data->inFunc = false;
  Data->followingMlExprEOL = false;
  Data->indexif = 0;
  Data->writef = false;

  Stack_Init(&Data->SymStack, MAX_ARGS);
}

void DisposeParserData(ParserDataT *Data)
{
  for (int i = 0; i <= Data->max_scope; i++)
  {
      avl_dispose(&Data->LocalST[i]);
  }

  free(Data->LocalST);
  free(Data->code);
  FlushToken(Data->token);
  free(Data->token);

  Stack_Dispose(&Data->SymStack);
}

int loadFuns(ParserDataT *gData)
{
    GET_TOKEN();
    while (gData->token->type != EOFILE)
    {
        if (gData->token->type == FUNC)
        {
            GET_TOKEN();
            if (gData->token->type == IDENTIFICATOR)
            {
                strcpy(gData->symTableData.identifier, gData->token->fluidPayload->payload);
                gData->symTableData.type = TYPE_FUNCTION;
                while (gData->token->type != CLBR)
                {
                    GET_TOKEN();
                }
                GET_TOKEN();
                if (gData->token->type == LAMBDA)
                {
                    GET_TOKEN();
                    switch (gData->token->type)
                    {
                    case INTKEY:
                        gData->symTableData.params[0] = 'i';
                        break;
                    case DOUBLEKEY:
                        gData->symTableData.params[0] = 'd';
                        break;
                    case STRINGKEY:
                        gData->symTableData.params[0] = 's';
                        break;
                    default:
                        break;
                    }
                    gData->symTableData.params[1] = '\0';
                    GET_TOKEN();
                    if (gData->token->type == QUESTIONMARK)
                    {
                        gData->symTableData.optional = true;
                    }
                }
                else
                {
                    gData->symTableData.params[0] = 'v';
                    gData->symTableData.params[1] = '\0';
                }
                INSERT_SYMBOL(gData->LocalST, gData->current_scope[gData->top], gData->symTableData.identifier, gData->symTableData);
                TSDATA_RESET(gData);
                for (int i = 0; i < MAX_ARGS - 1; i++)
                {
                    gData->symTableData.params[i] = '\0';
                }
            }
        }
        GET_TOKEN();
    }
    Return2Begg();
    return 0;
}

// starting point
int parse(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "PARSE");
    result = start(gData);
    if (result == SYN_OK)
        return 0;

    return result;
}

// <START>
int start(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "START");

    gData->argExpr = false; // Initilization, has to be set to false!
    gData->inFunc = false;  // Initilization, has to be set to false!

    GET_TOKEN();

    // <START> -> <STAT> <START>
    if (IS_STAT_FIRST(gData->token->type))
    {
        gData->pushBack = true;
        EXEC_RULE(stat);
        
        return start(gData);
    }

    // <START> -> func id (<PL>) <FUN_TYPE> {<FUN_BODY>} EOL <START>
    if (gData->token->type == FUNC)
    {
        GET_TOKEN_AND_EXPECT_TYPE(IDENTIFICATOR);
        generator_func_header(gData->code,gData->token->fluidPayload->payload);
        char*array= gData->token->fluidPayload->payload;

        strcpy(gData->symTableData.identifier, gData->token->fluidPayload->payload);
        gData->symTableData.type = TYPE_FUNCTION;

        GET_TOKEN_AND_EXPECT_TYPE(OPBR);
        EXEC_RULE(pl);
        GET_TOKEN_AND_EXPECT_TYPE(CLBR);

        EXEC_RULE(funType);

        char type;
        type = gData->symTableData.params[0];

        GET_TOKEN_AND_EXPECT_TYPE(OPCB);
        gData->symTableData.defined = true;
        //INSERT_SYMBOL(gData->LocalST, gData->current_scope[gData->top], gData->symTableData.identifier, gData->symTableData);
        avl_Data *function = search_symbol(gData, gData->symTableData.identifier);
        if (function != NULL)
        {
            function->constant = gData->symTableData.constant;
            function->defined = gData->symTableData.defined;
            function->initialized = gData->symTableData.initialized;
            function->optional = gData->symTableData.optional;
            function->underscore = gData->symTableData.underscore;
            for (int i = 0; i < strlen(gData->symTableData.params); i++)
            {
                function->params[i] = gData->symTableData.params[i];
            }
            for (int i = 0; i < strlen(gData->symTableData.params) - 1; i++)
            {
                function->param_ids[i] = gData->symTableData.param_ids[i];
                function->underscore_param[i] = gData->symTableData.underscore_param[i];
            }
        }
        
        TSDATA_RESET(gData);
        gData->symTableData.type = TYPE_INT;
        for (int i = 0; i < MAX_ARGS - 1; i++)
        {
            gData->symTableData.params[i] = '\0';
        }

        NEW_SCOPE(gData->LocalST, gData->max_scope, gData->current_scope, gData->top);
        int result = add_params(gData);
        if (result != 0)
        {
            return result;
        }
        for (int i = 0; i < gData->number_params - 1; i++)
        {
            gData->symTableData.underscore_param[i] = false;
        }
        gData->number_params = 0;
        TSDATA_RESET(gData);

        //EXEC_RULE(funBody, type);
        result = funBody(gData, type);
        if (result != SYN_OK)
            return result;

        GET_TOKEN_AND_EXPECT_TYPE(CLCB);
       
        generator_func_end(gData->code,array);

        END_OF_SCOPE(gData->top);
        
        if (gData->token->followingEOL != true)
            return SYN_ERR;

        return start(gData);
    }

    // <START> -> EOF
    if (gData->token->type == EOFILE)
    {
        return SYN_OK;
    }

    return SYN_ERR;
}

// <PL>
int pl(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "PL");

    GET_TOKEN();

    // <PL> -> <TERM> : <DATA_TYPE> <PL2>
    if (gData->token->type == UNDERSCORE || gData->token->type == IDENTIFICATOR)
    {        
        gData->pushBack = true;
        EXEC_RULE(term);

        GET_TOKEN_AND_EXPECT_TYPE(DOUBLEDOT);

        EXEC_RULE(dataType);

        return pl2(gData);
    }

    // <PL> -> epsilon
    if (gData->token->type == CLBR)
    {
        gData->pushBack = true;

        return SYN_OK;
    }
    
    return SYN_ERR;
}

// TERM
int term(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "TERM");

    GET_TOKEN();

    // <TERM> -> _ id
    if (gData->token->type == UNDERSCORE)
    {
        GET_TOKEN_AND_EXPECT_TYPE(IDENTIFICATOR);
        gData->symTableData.underscore_param[gData->number_params++] = true;
        Stack_Push(&gData->SymStack, *gData->token);
        generator_func_params(gData->code,gData->token->fluidPayload->payload);
        return SYN_OK;
    }

    // <TERM> -> id id
    if (gData->token->type == IDENTIFICATOR)
    {
        gData->symTableData.underscore_param[gData->number_params] = false;
        gData->symTableData.param_ids[gData->number_params] = (char*)calloc(ID_MAX_LEN, sizeof(char));
        strcpy(gData->symTableData.param_ids[gData->number_params++], gData->token->fluidPayload->payload);
        GET_TOKEN_AND_EXPECT_TYPE(IDENTIFICATOR);
        if (strcmp(gData->symTableData.param_ids[gData->number_params - 1], gData->token->fluidPayload->payload));
        {
            return SEM_VAR_CNT_ERR;
        }
        
        Stack_Push(&gData->SymStack, *gData->token);
        generator_func_params(gData->code,gData->token->fluidPayload->payload);
       
        return SYN_OK;
    }

    return SYN_ERR;
}

// <DATA_TYPE>
int dataType(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "DATA_TYPE");

    GET_TOKEN();

    if (gData->symTableData.type != TYPE_FUNCTION)
    {
        switch (gData->token->type)
        {
        case INTKEY:
            gData->symTableData.type = TYPE_INT;
            break;
        case DOUBLEKEY:
            gData->symTableData.type = TYPE_DOUBLE;
            break;
        case STRINGKEY:
            gData->symTableData.type = TYPE_STRING;
            break;
        default:
            break;
        }
    }
    else
    {
        if (gData->Return_type != true)
        {
            Stack_Push(&gData->SymStack, *gData->token);
            int len = strlen(gData->symTableData.params);
            switch (gData->token->type)
            {
            case INTKEY:
                gData->symTableData.params[len] = 'i';
                break;
            case DOUBLEKEY:
                gData->symTableData.params[len] = 'd';
                break;
            case STRINGKEY:
                gData->symTableData.params[len] = 's';
                break;
            default:
                break;
            }
            gData->symTableData.params[len + 1] = '\0';
        }
        else
        {
            int len = strlen(gData->symTableData.params);
            gData->symTableData.params[len + 1] = '\0';
            for (int i = len; i > 0; i--)
            {
                gData->symTableData.params[i] = gData->symTableData.params[i - 1];
            }
            switch (gData->token->type)
            {
            case INTKEY:
                gData->symTableData.params[0] = 'i';
                break;
            case DOUBLEKEY:
                gData->symTableData.params[0] = 'd';
                break;
            case STRINGKEY:
                gData->symTableData.params[0] = 's';
                break;
            default:
                break;
            }
        }
        
    }

    // <DATA_TYPE> -> Int <QMARK> | Double <Q_MARK> | String <Q_MARK>
    if (gData->token->type == INTKEY || gData->token->type == DOUBLEKEY || gData->token->type == STRINGKEY)
    {
        result =  qMark(gData);
        return result;
    }

    return SYN_ERR;
}

// <Q_MARK>
int qMark(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "Q_MARK");
    
    // <Q_MARK> -> epsilon
    if (gData->token->followingEOL)
    {
        return SYN_OK;
    }

    GET_TOKEN();

    // <Q_MARK> -> ?
    if (gData->token->type == QUESTIONMARK)
    {
        gData->symTableData.optional = true;        // optional value, defined with '?'
        return SYN_OK;
    }
    
    // <Q_MARK> -> epsilon
    if (gData->token->type == CLBR || gData->token->type == OPCB || gData->token->type == EQ || gData->token->type == COMA)
    {
        gData->pushBack = true;
        return SYN_OK;
    }

    return SYN_ERR;
}

// <PL2>
int pl2(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "PL2");

    GET_TOKEN();

    // <PL2> -> , <TERM> : <DATA_TYPE> <PL2>
    if (gData->token->type == COMA)
    {
        EXEC_RULE(term);

        GET_TOKEN_AND_EXPECT_TYPE(DOUBLEDOT);

        EXEC_RULE(dataType);

        return pl2(gData);
    }

    // <PL2> -> epsilon
    if (gData->token->type == CLBR)
    {
       gData->pushBack = true;

       return SYN_OK;
    }

    return SYN_ERR;
}

// <FUN_TYPE>
int funType(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "FUN_TYPE");

    GET_TOKEN();
    
    // <FUN_TYPE> -> -> <DATA_TYPE>
    if (gData->token->type == LAMBDA)
    {
        gData->Return_type = true;      // Function has a return type
        return dataType(gData);
    }

    // <FUN_TYPE> -> epsilon
    if (gData->token->type == CLCB)              // Function doesn't have a return type
    {
        int len = strlen(gData->symTableData.params);
        gData->symTableData.params[len + 1] = '\0';
        for (int i = len; i > 0; i--)
        {
            gData->symTableData.params[i] = gData->symTableData.params[i - 1];
        }
        gData->symTableData.params[0] = 'v';

        gData->pushBack = true;
        return SYN_OK;
    }

    return SYN_ERR;
}

// <FUN_BODY>
int funBody(ParserDataT *gData, char type)
{
    gData->inFunc = true;
    DEBUG_PRINT("%s\n", "FUN_BODY");

    GET_TOKEN();
    
    // <FUN_BODY> -> return <VALUE>
    if (gData->token->type == RETURN)
    {
        
        GET_TOKEN();
        if(gData->token->type == CLCB)
        {
            if (type != 'v')
            {
                return SEM_FUN_RETURN_ERR;
            }
            gData->pushBack = true;
            
            return SYN_OK;
        }
        else
        {
            gData->pushBack = true;
            
            EXEC_RULE(value);
            
            switch (type)
            {
            case 'v':
                return SEM_FUN_RETURN_ERR;
                break;
            case 'i':
                if (gData->result_type != TYPE_INT)
                {
                    return SEM_VAR_CNT_ERR;
                }
                break;
            case 'd':
                if (gData->result_type != TYPE_DOUBLE)
                {
                    return SEM_VAR_CNT_ERR;
                }
                break;
            case 's':
                if (gData->result_type != TYPE_STRING)
                {
                    return SEM_VAR_CNT_ERR;
                }
                break;    
            default:
                return SEM_FUN_RETURN_ERR;
                break;
            }
        }

        return funBody(gData, type);
    }

    if (gData->token->type == CLCB)
    {
        gData->pushBack = true;
        return SYN_OK;
    }

    // <FUN_BODY> -> <STAT> <FUN_BODY>
    if (IS_STAT_FIRST(gData->token->type))
    {
        gData->pushBack = true;

        EXEC_RULE(stat);
        return funBody(gData, type);
    }

    return SYN_ERR;
}

// <STAT>
int stat(ParserDataT *gData)
{ 
    DEBUG_PRINT("%s\n", "STAT");
    GET_TOKEN();

    // <STAT> -> if <IF_H> { <STAT_LIST> } else { <STAT_LIST> } EOL
    if (gData->token->type == IF)
    {   
        gData->indexif = gData->indexif+1;
        int index = gData->indexif;
        gData->argExpr = false;
        EXEC_RULE(ifH);
        generator_if_header(gData->code);
        NEW_SCOPE(gData->LocalST, gData->max_scope, gData->current_scope, gData->top);
        generator_if_jump_to_else(gData->code,index);
        GET_TOKEN_AND_EXPECT_TYPE(OPCB);

        EXEC_RULE(statList);
       

        GET_TOKEN_AND_EXPECT_TYPE(CLCB);

        generator_if_jump_to_skip(gData->code,index);
        generator_if_label_else(gData->code,index);

        if (gData->IfLetVar != NULL)
        {
            gData->IfLetVar->optional = true;
        }
        
        END_OF_SCOPE(gData->top);

        GET_TOKEN_AND_EXPECT_TYPE(ELSE);

        NEW_SCOPE(gData->LocalST, gData->max_scope, gData->current_scope, gData->top);

        GET_TOKEN_AND_EXPECT_TYPE(OPCB);
        EXEC_RULE(statList); 
        
        GET_TOKEN_AND_EXPECT_TYPE(CLCB);
        generator_if_label_skip(gData->code,index);


        END_OF_SCOPE(gData->top);

        if (gData->token->followingEOL)
            return SYN_OK;

        return SYN_ERR;
    }

    // <STAT> -> while (<EXPR>) { <STAT_LIST> } EOL
    if (gData->token->type == WHILE)
    {
        gData->indexif = gData->indexif+1;
        int index = gData->indexif;

        generator_while_start(gData->code,index);
        gData->argExpr = false;
        GET_TOKEN_AND_EXPECT_TYPE(OPBR);

        result = expr(gData, NULL, 0);
        if (result != SYN_OK)
            return result;

        // CLBR handled by expression parser.
        //GET_TOKEN_AND_EXPECT_TYPE(CLBR);
        generator_while_save(gData->code);
        generator_while_jump_to_skip(gData->code,index);

        NEW_SCOPE(gData->LocalST, gData->max_scope, gData->current_scope, gData->top);

        GET_TOKEN_AND_EXPECT_TYPE(OPCB);
        EXEC_RULE(statList);
        GET_TOKEN_AND_EXPECT_TYPE(CLCB);
        
        generator_while_jump_to_start(gData->code,index);
        generator_while_label_skip(gData->code,index);
        END_OF_SCOPE(gData->top);

        if (gData->token->followingEOL)
            return SYN_OK;

        return SYN_ERR;
    }

    // <STAT> -> <VAR_DEC> id : <DATA_TYPE> <ASSIGN> EOL
    if (gData->token->type == LET || gData->token->type == VAR)
    {
        gData->pushBack = true;
        EXEC_RULE(varDec);

        GET_TOKEN_AND_EXPECT_TYPE(IDENTIFICATOR);
    
        strcpy(gData->symTableData.identifier, gData->token->fluidPayload->payload);

        GET_TOKEN();
        bool prevTokenFollowingEOL = false;
        // Missing in LL grammar <STAT> -> <VAR_DEC> id <ASSIGN> EOL
        if (gData->token->type == EQ)
        {
            gData->pushBack = true;
            EXEC_RULE(assign);
        }
        else if (gData->token->type == DOUBLEDOT)
        {
            EXEC_RULE(dataType);
            gData->type_assigned = true;

            prevTokenFollowingEOL = gData->token->followingEOL;
            GET_TOKEN();
            gData->pushBack = true;

            if (gData->token->type == EQ)
            {
                EXEC_RULE(assign);
            }
            else if (!prevTokenFollowingEOL)
            {
                return SYN_ERR;
            }

        }
        else
        {
            return SYN_ERR;
        }

        
        if (gData->assign == true)
        {
            if (gData->type_assigned == true)
            {
                if (gData->result_type == TYPE_INT && gData->CanConvertToDouble == true)
                {
                }
                else if (gData->symTableData.type != gData->result_type)
                {
                    return SEM_TYPES_AR_ERR;
                }
            }
            else
            {
                gData->symTableData.type = gData->result_type;
            }
        }

        avl_Data *res = avl_search(gData->LocalST[gData->current_scope[gData->top]], gData->symTableData.identifier);
        avl_Data*id_node= search_symbol(gData,gData->symTableData.identifier);
        INSERT_SYMBOL(gData->LocalST, gData->current_scope[gData->top], gData->symTableData.identifier, gData->symTableData);
        avl_Data*id_node_after= search_symbol(gData,gData->symTableData.identifier);
         
        if (id_node != NULL && res == NULL){
            // redaclare
            generator_var_asign(gData->code,id_node_after);
        }

        if(id_node == NULL && res == NULL){
            generator_var_define(gData->code,id_node_after);
            generator_var_asign(gData->code,id_node_after);
       }
    

        TSDATA_RESET(gData);

        if (gData->token->followingEOL || gData->followingMlExprEOL
            || prevTokenFollowingEOL)
        {
            gData->followingMlExprEOL = false;
            
            return SYN_OK;
        }

        return SYN_ERR;
    }

    // <STAT> -> id <FUN_VAR> EOL
    if (gData->token->type == IDENTIFICATOR)
    {
        avl_Data*id_node= search_symbol(gData,gData->token->fluidPayload->payload);

        if(id_node->type == 0)
            {
                gData->writef = true;
            }

        EXEC_RULE(funVar);


        if (gData->token->followingEOL || gData->followingMlExprEOL)
        {
            gData->followingMlExprEOL = false;
            return SYN_OK;
        }

        GET_TOKEN();
        // if the line is ending with } or {.
        if (gData->token->type == CLCB || gData->token->type == OPCB)
        {
            gData->pushBack = true;
            return SYN_OK;
        }

        return SYN_ERR;
    }

    return SYN_ERR;
}

// <IF_H>
int ifH(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "IF_H");

    GET_TOKEN();

    // <IF_H> -> (<EXPR>)
    if (gData->token->type == OPBR)
    {
        result = expr(gData, NULL, 0);
        if (result != SYN_OK)
            return result;

        // CLBR handled by expression parser.
        //GET_TOKEN_AND_EXPECT_TYPE(CLBR);
        
        return SYN_OK;
    }

    // <IF_H> -> let id
    if (gData->token->type == LET)
    {
        GET_TOKEN_AND_EXPECT_TYPE(IDENTIFICATOR);

        avl_Data* data = search_symbol(gData, gData->token->fluidPayload->payload);
        generator_if_let(gData->code,data->identifier);
        data->optional = false;
        gData->IfLetVar = data;
        return SYN_OK;
    }

    return SYN_ERR;
}

int statList(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "STAT_LIST");

    GET_TOKEN();
    
    // <STAT_LIST> -> <STAT> <STAT_LIST>
    if (IS_STAT_FIRST(gData->token->type))
    {
        gData->pushBack = true;
        EXEC_RULE(stat);

        return statList(gData);
    }
    
    // <STAT_LIST> -> epsilon
    if (gData->token->type == CLCB)
    {
        gData->pushBack = true;
        return SYN_OK;
    }

    // <STAT_LIST> -> return <VALUE>
    if (gData->token->type == RETURN && gData->inFunc)
    {
       result = value(gData);

       if (result != SYN_OK)
           return result;

       return statList(gData);
    }

    return SYN_ERR;
}

//<VAR_DEC>
int varDec(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "VAR_DEC");

    GET_TOKEN();

    // <VAR_DEC> -> let || var
    if (gData->token->type == LET || gData->token->type == VAR)
    {
        if (gData->token->type == LET)
        {
            gData->symTableData.constant = true;
        }
        
        return SYN_OK;
    }

    return SYN_ERR;
}

// <ASSIGN>
int assign(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "ASSIGN");
    
    GET_TOKEN();
    if (gData->token->type == EQ)
    {
        gData->assign = true;
        return value(gData);
    }

    // <ASSIGN> -> epsilon
    if (gData->token->followingEOL)
    {
        return SYN_OK;
    }

    return SYN_ERR;
}

// <VALUE>
int value(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "VALUE");
    GET_TOKEN();
    
    if (IS_EXPR_FIRST(gData->token->type))
    {
        Token backTokens[1] = {*(gData->token)}; 

        if(gData->writef == true)
            generator_write(gData->code,gData->token);

        result = expr(gData, backTokens, 1);
        return result;
    }

    // <VALUE> -> id <ARGS>
    if (gData->token->type == IDENTIFICATOR)
    {
        Token tok1 = *(gData->token);

        avl_Data *function = search_symbol(gData, gData->token->fluidPayload->payload);

        GET_TOKEN();
        
        if (gData->token->type == OPBR)
        {
            if (function == NULL)
            {
                if (gData->type_assigned != true)
                {
                    return SEM_UF_ERR;
                }
            }
            else
            {
                switch (function->params[0])
                {
                case 'v':
                    gData->result_type = TYPE_UNDEFINED;
                    break;
                case 'i':
                    gData->result_type = TYPE_INT;
                    break;
                case 'd':
                    gData->result_type = TYPE_DOUBLE;
                    break;
                case 's':
                    gData->result_type = TYPE_STRING;
                    break;
                default:
                    break;
                }
                if (function->optional == true)
                {
                    gData->symTableData.optional = true;
                }
                
            }
            gData->pushBack = true;
            gData->argExpr = false;
            

            int argResponse =  args(gData);   
                generator_func_call(gData->code,function->identifier);
                generator_save_return(gData->code);
                //generator_save_exp(gData->code);
        
            return argResponse;
        }

        gData->followingMlExprEOL = true;
        Token backTokens[2] = {*(gData->token), tok1};
        result = expr(gData, backTokens, 2);
        
        if(gData->writef == true)
                generator_write(gData->code,gData->token);

        //generator_var_asign_fluid(gData->code,function->identifier);
        return result;
    }
    return SYN_ERR;
}

// <P_VALUE>
int pValue(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "P_VALUE");
    
    
    GET_TOKEN();


    if (gData->token->type == IDENTIFICATOR)
    {
        Token fToken = *(gData->token);

         if(gData->writef)
                generator_write(gData->code,gData->token); // todo MOZAN POSUNOUT DOLU ABY UMELO EXPRESSION

        GET_TOKEN();
        
        // <P_VALUE> -> id : <VALUE>
        if (gData->token->type == DOUBLEDOT)
        {
            return value(gData);
        }
        // Empty arguments
        else if (gData->token->type == CLBR)
        {
            gData->pushBack = true;
            return SYN_OK;
        }
        else if (gData->token->type == COMA)
        {
            gData->pushBack = true;
            return SYN_OK;
        }
        else
        {

            Token backTokens[2] = {*(gData->token), fToken}; 
            return expr(gData, backTokens, 2);
        }
    }
    // <P_VALUE> -> <VALUE>
    else if (IS_VALUE(gData->token->type))
    {
        gData->pushBack = true;
        return value(gData);
    }

    return SYN_ERR;
}

// <FUN_VAR>
int funVar(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "FUN_VAR");

    strcpy(gData->symTableData.identifier, gData->token->fluidPayload->payload);
   
    GET_TOKEN();

    // <FUN_VAR> -> <VALUE>
    if (gData->token->type == EQ)
    {
        result = value(gData);
        avl_Data *variable = search_symbol(gData, gData->symTableData.identifier);
        generator_save_exp(gData->code);
        generator_var_asign_fluid(gData->code,variable->identifier);
        if (variable != NULL)
        {
            if (gData->result_type == TYPE_INT && gData->CanConvertToDouble == true)
            {
            }
            else if (variable->type != gData->result_type)
            {
                return SEM_TYPES_AR_ERR;
            }
            else if (gData->symTableData.optional == true && variable->optional == false)
            {
                return SEM_TYPES_AR_ERR;
            }
        }
        return result;
    }

    // <FUN_VAR> -> (<P_VALUE> <ARGS2>)
    if (gData->token->type == OPBR)
    {
        gData->argExpr = true;      // To help expressing expressions that are function argument.
        EXEC_RULE(pValue);
        EXEC_RULE(args2);

        GET_TOKEN_AND_EXPECT_TYPE(CLBR);
        gData->writef = false;
        return SYN_OK;
    }

    return SYN_ERR;
}

// <ARGS>
int args(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "ARGS");

    // <ARGS> -> epsilon
    if (gData->token->followingEOL)
    {
        return SYN_OK;
    }
    
    GET_TOKEN();
    
    // <ARGS> -> (<P_VALUE> <ARGS2>)
    if (gData->token->type == OPBR)
    {
        gData->argExpr = true;      // To help express expressions that are function arguments.
        GET_TOKEN();
        if(search_symbol(gData,gData->token->fluidPayload->payload)) // TODO 
        {   
                generator_pass_arguments_to_func(gData->code,gData->token->fluidPayload->payload);     
        }
        // When ARGUMENTS are empty - ()
        if (gData->token->type == CLBR)
        {
            return SYN_OK;
        }
       
        gData->pushBack = true;
       
        EXEC_RULE(pValue);
        EXEC_RULE(args2);
        GET_TOKEN_AND_EXPECT_TYPE(CLBR);

        return SYN_OK;
    }

    // <ARGS> -> epsilon
    if (gData->token->type == COMA)
    {
        gData->pushBack = true;

        return SYN_OK;
    }

    return SYN_ERR;
}

// <ARGS2>
int args2(ParserDataT *gData)
{
    DEBUG_PRINT("%s\n", "ARGS2");
    GET_TOKEN();
    // <ARGS2> -> , <P_VALUE> <ARGS2>
    if (gData->token->type == COMA)
    {
        EXEC_RULE(pValue);
        return args2(gData);
    }

    // <ARGS2> -> epsilon
    if (gData->token->type == CLBR)
    {
        gData->pushBack = true;

        return SYN_OK;
    }

    return SYN_ERR;
}

int expr(ParserDataT *gData, Token pushedTokens[], int tokensCnt)
{
    DEBUG_PRINT("%s\n", "EXPR");

    if (gData == NULL)
        return SYN_ERR;

    int result =  parseExpression(gData, pushedTokens, tokensCnt, gData->argExpr); 

    return result;
}
/*
*
*   Returns NULL if the variable/function hasn't been defined, otherwise returns pointer to the data of the record in the symtable
*/
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

int add_params(ParserDataT *gData)
{
    while (Stack_IsEmpty(&gData->SymStack) == false)
    {
        Token token;
        int i = gData->number_params - 1;
        Stack_Top(&gData->SymStack, &token);
        Stack_Pop(&gData->SymStack);
        switch (token.type)
        {
        case INTKEY:
            gData->symTableData.type = TYPE_INT;
            break;
        case DOUBLEKEY:
            gData->symTableData.type = TYPE_DOUBLE;
            break;
        case STRINGKEY:
            gData->symTableData.type = TYPE_STRING;
            break;
        default:
            break;
        }
        if (gData->symTableData.underscore_param[i] == true)
        {
            gData->symTableData.underscore = true;
        }
        else
        {
            gData->symTableData.underscore = false;
        }
        gData->symTableData.initialized = true;
        gData->symTableData.defined = true;
        Stack_Top(&gData->SymStack, &token);
        Stack_Pop(&gData->SymStack);
        strcpy(gData->symTableData.identifier, token.fluidPayload->payload);
        generator_func_pops(gData->code,gData->symTableData.identifier);
        avl_Data *result = avl_search(gData->LocalST[gData->current_scope[gData->top]], gData->symTableData.identifier);
        if (result != NULL)     // check for redefinition of a parameter
            return SEM_UF_ERR;
        gData->LocalST[gData->current_scope[gData->top]] = avl_insert(gData->LocalST[gData->current_scope[gData->top]], gData->symTableData.identifier, gData->symTableData);
    }
    return 0;
}

int sem_anal(ParserDataT *gData)
{
    Return2Begg();
    gData->top = 0;
    gData->max_scope = 0;
    gData->current_scope[0] = 0;
    Token token_prev;
    bool new_token = false;
    bool function_call = false;
    CALL_GET_TOKEN();

    while (gData->token->type != EOFILE)
    {
        if (gData->token->type == LET)
        {
            if (token_prev.type == IF)      // if let id
            {
                NEW_TOKEN();
                avl_Data *variable = search_symbol(gData, gData->token->fluidPayload->payload);
                if (variable == NULL)
                {
                    return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                }
                else if (variable->defined != true)
                {
                    return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                }
                else if (variable->optional != true)
                {
                    return SEM_OTHER_ERR;  // Variable is not optional
                }
                else if (variable->initialized != true)
                {
                    return SEM_UNDEF_VAR_ERR;  // Use of uninitialized variable
                }
            }
        }
        else if (gData->token->type == IDENTIFICATOR)
        {
            avl_Data *data;
            switch (token_prev.type)
            {
            case LET:
            case VAR:
                // Definition of a variable
                data = search_symbol(gData, gData->token->fluidPayload->payload);
                if (data == NULL)
                {
                    return SEM_OTHER_ERR;
                }
                
                data->defined = true;
                NEW_TOKEN();
                if (gData->token->type == EQ)
                {
                    data->initialized = true;
                }
                else if (gData->token->type == DOUBLEDOT)
                {
                    NEW_TOKEN();
                    if (gData->token->type == INTKEY || gData->token->type == DOUBLEKEY || gData->token->type == STRINGKEY)
                    {
                        NEW_TOKEN();
                        if (gData->token->type == EQ)
                        {
                            data->initialized = true;
                        }
                        else if (gData->token->type == QUESTIONMARK)
                        {
                            NEW_TOKEN();
                            if (gData->token->type == EQ)
                            {
                                data->initialized = true;
                            }
                            else
                            {
                                new_token = true;
                            }
                        }
                    }
                }
                else
                {
                    new_token = true;
                }
                break;
            // Definition of a function
            case FUNC:
                data = search_symbol(gData, gData->token->fluidPayload->payload);
                data->defined = true;
                while (gData->token->type != CLBR)
                {
                    NEW_TOKEN();
                }
                break;
            default:
                if (function_call == false)
                {
                    NEW_TOKEN();
                }
                if (gData->token->type == OPBR || function_call == true)  // Function call
                {
                    function_call = false;
                    data = search_symbol(gData, token_prev.fluidPayload->payload);
                    if (data == NULL)
                    {
                        return SEM_UF_ERR;  // Function is undefined
                    }
                    int num_params = 1;
                    NEW_TOKEN();
                    while (gData->token->type != CLBR)  // Parameters
                    {
                        if (data->params[1] == 'x')  // variable number of parameters
                        {
                            NEW_TOKEN();
                            continue;
                        }
                        if (data->underscore_param[num_params - 1] == true)     // _id
                        {
                            Token backTokens[1] = {*(gData->token)};
                            gData->argExpr = true;
                            result = expr(gData, backTokens, 1);
                            if (result != 12)
                            {
                                return result;
                            }
                            switch (data->params[num_params])
                            {
                            case 'i':
                                if (gData->result_type != TYPE_INT)
                                {
                                    return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                }
                                break;
                            case 'd':
                                if (gData->result_type != TYPE_DOUBLE)
                                {
                                    if (gData->result_type == TYPE_INT && gData->CanConvertToDouble == true)
                                    {
                                    }
                                    else
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                }
                                break;
                            case 's':
                                if (gData->result_type != TYPE_STRING)
                                {
                                    return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                }
                                break;    
                            }
                            num_params++;
                            if (gData->token->type == CLBR)
                            {
                                break;
                            }
                        }
                        else
                        {
                            if (gData->token->type == IDENTIFICATOR)
                            {
                                if (strcmp(data->param_ids[num_params - 1], gData->token->fluidPayload->payload) != 0)
                                {
                                    return SEM_VAR_CNT_ERR;
                                }
                                NEW_TOKEN();    // ':'
                                if (gData->token->type != DOUBLEDOT)
                                {
                                    return SEM_VAR_CNT_ERR;
                                }
                                NEW_TOKEN();
                                gData->argExpr = true;
                                Token backTokens[1] = {*(gData->token)}; 
                                result = expr(gData, backTokens, 1);
                                if (result != 12)
                                {
                                    return result;
                                }
                                switch (data->params[num_params])
                                {
                                case 'i':
                                    if (gData->result_type != TYPE_INT)
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                    break;
                                case 'd':
                                    if (gData->result_type != TYPE_DOUBLE)
                                    {
                                        if (gData->result_type == TYPE_INT && gData->CanConvertToDouble == true)
                                        {
                                        }
                                        else
                                        {
                                            return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                        }
                                    }
                                    break;
                                case 's':
                                    if (gData->result_type != TYPE_STRING)
                                    {
                                        return SEM_VAR_CNT_ERR;  // Wrong data type of a parameter
                                    }
                                    break;    
                                }
                                num_params++;
                                if (gData->token->type == CLBR)
                                {
                                    break;
                                }
                            }
                        }
                        NEW_TOKEN();
                    }
                    if (data->params[1] != 'x')  // variable number of parameters
                    {
                        if ((int)strlen(data->params) != num_params)
                        {
                            return SEM_VAR_CNT_ERR;  // Wrong number of parameters
                        }
                    }
                }
                else if (gData->token->type == EQ)  // assignment to a variable
                {
                    if (token_prev.type == IDENTIFICATOR)
                    {
                        avl_Data *variable = search_symbol(gData, token_prev.fluidPayload->payload);
                        if (variable == NULL)
                        {
                            return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                        }
                        else if (variable->defined == false)
                        {
                            return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                        }
                        else if (variable->constant == true)
                        {
                            return SEM_OTHER_ERR;  // Can't assing to a constant
                        }

                        NEW_TOKEN();
                        if (gData->token->type == IDENTIFICATOR)
                        {
                            NEW_TOKEN();
                            if (gData->token->type == OPBR)
                            {
                                avl_Data *function = search_symbol(gData, token_prev.fluidPayload->payload);
                                if (function == NULL)
                                {
                                    return SEM_UF_ERR;  // Function is undefined
                                }
                                if (function->params[0] == 'v')  // Funkce nema navratovy typ
                                {
                                    return SEM_OTHER_ERR;
                                }
                                else if (function->params[0] == 'i')
                                {
                                    if (variable->type != TYPE_INT)
                                    {
                                        return SEM_TYPES_AR_ERR;
                                    }
                                }
                                else if (function->params[0] == 'd')
                                {
                                    if (variable->type != TYPE_DOUBLE)
                                    {
                                        return SEM_TYPES_AR_ERR;
                                    }
                                }
                                else if (function->params[0] == 's')
                                {
                                    if (variable->type != TYPE_STRING)
                                    {
                                        return SEM_TYPES_AR_ERR;
                                    }
                                }
                                if (function->optional == true)
                                {
                                    variable->optional = true;
                                }
                                function_call = true;
                                gData->token = &token_prev;
                            }
                        }
                        variable->initialized = true;
                        new_token = true;
                    }
                }
                else    // Use of a variable
                {
                    avl_Data *variable = search_symbol(gData, token_prev.fluidPayload->payload);

                    if (variable == NULL)
                    {
                        return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                    }
                    else if (variable->defined == false)
                    {
                        return SEM_UNDEF_VAR_ERR;  // Use of undefined variable
                    }
                    else if (variable->initialized == false)
                    {
                        return SEM_UNDEF_VAR_ERR;  // Use of uninitialized variable
                    }
                    new_token = true;
                }
                break;
            }
        }
        else if (gData->token->type == OPCB)  // new scope
        {
            gData->top++;
            gData->max_scope++;
            gData->current_scope[gData->top] = gData->max_scope;
        }
        else if (gData->token->type == CLCB)  // end of scope
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

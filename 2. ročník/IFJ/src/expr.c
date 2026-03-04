/**
 * @file expr.c
 *
 * @brief Implements expression analysis and code generator.
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 * @author Dominik Honza <xhonza04@stud.fit.vutbr.cz>
 * @author David Nepraš <xnepra02@stud.fit.vutbr.cz>
 */


#include "expr.h"
#include "generator.h"

// Debug PRINT
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#define FREE_STACK(_stack)                              \
    StackChar_Dispose(_stack);                          \
    free(_stack)

#define GET_TOP_AND_POP(_stack) do                      \
    {if ((StackChar_Top(_stack)) == NULL)               \
        return SYN_ERR;                                 \
     top = *(StackChar_Top(_stack));                    \
     StackChar_Pop(_stack);                             \
    }while(0)                                               

#define EXPECT_BREAK_AND_PUSH(_stack, _toPush, type, optional, literal) do                   \
    {GET_TOP_AND_POP(_stack);                                       \
     if (top.value == BREAK_E)                                      \
     {                                                              \
         StackChar_Push(_stack, _toPush, type, optional, literal);    \
         return SYN_OK;                                             \
     }                                                              \
     return SYN_ERR;                                                \
    }while(0)                                           

char precTable[9][9] = 
{
/// R - >, P - <, E - =, X - blank
///
///           +-    */   rel   (    )    ??   !    id   $
/*{+, -}*/    {'R', 'P', 'R', 'P', 'R', 'R', 'P', 'P', 'R'},
/*{*, /}*/    {'R', 'R', 'R', 'P', 'R', 'R', 'P', 'P', 'R'},
/* {rel}*/    {'P', 'P', 'X', 'P', 'R', 'R', 'P', 'P', 'R'},
/*  (   */    {'P', 'P', 'P', 'P', 'E', 'P', 'P', 'P', 'X'},
/*  )   */    {'R', 'R', 'R', 'x', 'R', 'R', 'R', 'X', 'R'},
/*  ??  */    {'P', 'P', 'P', 'P', 'R', 'P', 'P', 'P', 'R'},
/*  !   */    {'R', 'R', 'R', 'X', 'R', 'R', 'X', 'X', 'R'},
/*  id  */    {'R', 'R', 'R', 'X', 'R', 'R', 'R', 'X', 'R'},
/*  $   */    {'P', 'P', 'P', 'P', 'X', 'P', 'P', 'P', 'X'}
};

// Get PrecSym value based on the token_type
PrecSym getPrecSym(Token_Type type)
{
    switch (type)
    {
        case PLUS:
            return PLUS_E;
        case MINUS:
            return MIN_E;
        case MUL:
            return MUL_E;
        case DIV:
            return DIV_E;
        case LESSER:
            return LESS_E;
        case GREATER:
            return MORE_E;
        case LESSEREQ:
            return LEQ_E;
        case GREATEREQ:
            return MEQ_E;
        case EQEQ:
            return EQ_E;
        case NEQ:
            return NEQ_E;
        case OPBR:
            return OPBR_E;
        case CLBR:
            return CLBR_E;
        case DOUBLEQUESTIONMARK:
            return ISNIL_E;
        case EXCLAMATION:
            return EXC_E;
        case WHOLENUMBER:
            return WHOLENUMBER_E;
        case DECIMALNUMBER:
            return DECIMALNUMBER_E;
        case STRINGVALUE:
            return STRINGVALUE_E;
        case IDENTIFICATOR:
            return ID_E;
        default:
            return DOLLAR_E;
    }
}

bool hasValue(PrecSym sym)
{
    if (sym == ID_E || sym == WHOLENUMBER_E || sym == DECIMALNUMBER_E ||
        sym == STRINGVALUE_E)
        return true;

    return false;
}
// Get index in precedence table based on parameter sym
int getTableIndex(PrecSym sym)
{
    switch (sym)
    {
        case PLUS_E:
        case MIN_E:
            return 0;
        case MUL_E:
        case DIV_E:
            return 1;
        case LESS_E:
        case MORE_E:
        case LEQ_E:
        case MEQ_E:
        case EQ_E:
        case NEQ_E:
            return 2;
        case OPBR_E:
            return 3;
        case CLBR_E:
            return 4;
        case ISNIL_E:
            return 5;
        case EXC_E:
            return 6;
        case ID_E:
        case WHOLENUMBER_E:
        case DECIMALNUMBER_E:
        case STRINGVALUE_E:
            return 7;
        case DOLLAR_E:
            return 8;
        default:
            return 8;
    }
}

// Checks Binary operator.
// TODO: Include semantic analysis here
int checkBinOperator(StackChar *stack, StackCharItem item, ParserDataT *gData)
{
    // item = Right operand, top = Left operand
    StackCharItem top;

    GET_TOP_AND_POP(stack);
    
    // If the second operand is not EXPR_E
    if (top.value != EXPR_E)
        return SYN_ERR;


    switch (item.value)
    {
        // String, Double and Int allowed for these operators
        case PLUS_E:
            if (top.type == TYPE_STRING && item.type == TYPE_STRING)    // string + string
            {
                if (gData->result_type == TYPE_DOUBLE || gData->result_type == TYPE_DOUBLE)
                {
                    return SEM_TYPES_AR_ERR;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                gData->result_type = TYPE_STRING;
                return SYN_OK;
            }
            else if (top.type == TYPE_DOUBLE && item.type == TYPE_DOUBLE)   // double + double
            {
                if (gData->result_type == TYPE_STRING)
                {
                    return SEM_TYPES_AR_ERR;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                gData->result_type = TYPE_DOUBLE;
                return SYN_OK;
            }
            else if (top.type == TYPE_INT && item.type == TYPE_INT)     // int + int
            {
                if (gData->result_type == TYPE_STRING)
                {
                    return SEM_TYPES_AR_ERR;
                }
                else if (gData->result_type != TYPE_DOUBLE)
                {
                    gData->result_type = TYPE_INT;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_DOUBLE && item.type == TYPE_INT)      // double + int
            {
                if (item.literal == true)
                {
                    if (gData->result_type == TYPE_STRING)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    if (top.optional != item.optional)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    gData->result_type = TYPE_DOUBLE;
                    return SYN_OK;
                }
            }
            else if (top.type == TYPE_INT && item.type == TYPE_DOUBLE)      // int + double
            {
                if (top.literal == true)
                {
                    if (gData->result_type == TYPE_STRING)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    if (top.optional != item.optional)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    gData->result_type = TYPE_DOUBLE;
                    return SYN_OK;
                }
            }
            return SEM_TYPES_AR_ERR;
        case LESS_E:
        case LEQ_E:
        case MORE_E:
        case MEQ_E:
            if (top.type == TYPE_STRING && item.type == TYPE_STRING)     // string < string
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_DOUBLE && item.type == TYPE_DOUBLE)   // double < double
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_INT && item.type == TYPE_INT)     // int < int
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            return SEM_TYPES_AR_ERR;
        case EQ_E:
        case NEQ_E:
            if (top.type == TYPE_STRING && item.type == TYPE_STRING)     // string == string
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_DOUBLE && item.type == TYPE_DOUBLE)   // double == double
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_INT && item.type == TYPE_INT)     // int == int
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_DOUBLE && item.type == TYPE_INT)      // double == int
            {   
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                if (item.literal == true)
                {
                    return SYN_OK;
                }
            }
            else if (top.type == TYPE_INT && item.type == TYPE_DOUBLE)      // int == double
            {
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                if (top.literal == true)
                {
                    return SYN_OK;
                }
            }
            return SEM_TYPES_AR_ERR;
        // Only Double and Int allowed for these operators.
        case MIN_E:
        case MUL_E:
            if (top.type == TYPE_DOUBLE && item.type == TYPE_DOUBLE)   // double -/* double
            {
                if (gData->result_type == TYPE_STRING)
                {
                    return SEM_TYPES_AR_ERR;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                gData->result_type = TYPE_DOUBLE;
                return SYN_OK;
            }
            else if (top.type == TYPE_INT && item.type == TYPE_INT)     // int -/* int
            {
                if (gData->result_type == TYPE_STRING)
                {
                    return SEM_TYPES_AR_ERR;
                }
                else if (gData->result_type != TYPE_DOUBLE)
                {
                    gData->result_type = TYPE_INT;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            else if (top.type == TYPE_DOUBLE && item.type == TYPE_INT)      // double -/* int
            {
                if (item.literal == true)
                {
                    if (gData->result_type == TYPE_STRING)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    if (top.optional != item.optional)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    gData->result_type = TYPE_DOUBLE;
                    return SYN_OK;
                }
            }
            else if (top.type == TYPE_INT && item.type == TYPE_DOUBLE)      // int -/* double
            {
                if (top.literal == true)
                {
                    if (gData->result_type == TYPE_STRING)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    if (top.optional != item.optional)
                    {
                        return SEM_TYPES_AR_ERR;
                    }
                    gData->result_type = TYPE_DOUBLE;
                    return SYN_OK;
                }
            }
            return SEM_TYPES_AR_ERR;
        case DIV_E:
            if (top.type == TYPE_DOUBLE && item.type == TYPE_DOUBLE)   // double / double
            {
                if (gData->result_type == TYPE_STRING)
                {
                    return SEM_TYPES_AR_ERR;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                gData->result_type = TYPE_DOUBLE;
                return SYN_OK;
            }
            else if (top.type == TYPE_INT && item.type == TYPE_INT)     // int / int
            {
                if (gData->result_type == TYPE_STRING)
                {
                    return SEM_TYPES_AR_ERR;
                }
                else if (gData->result_type != TYPE_DOUBLE)
                {
                    gData->result_type = TYPE_INT;
                }
                if (top.optional != item.optional)
                {
                    return SEM_TYPES_AR_ERR;
                }
                return SYN_OK;
            }
            return SEM_TYPES_AR_ERR;
        // E ?? E. Left operand has to be nullable.
        case ISNIL_E:
            if (top.optional == true)
            {
                if (top.type != item.type)
                {
                    return SEM_TYPES_AR_ERR;
                }
                else if (item.optional == true)
                {
                    return SEM_TYPES_AR_ERR;
                }
            }
            else
            {
                return SEM_TYPES_AR_ERR;
            }
            gData->result_type = top.type;
            return SYN_OK;
        default:
            return SYN_ERR;
    }
}

// Function called to reduce rules
int reduceRule(StackChar *stack,Token token,ParserDataT*gData)
{
    DEBUG_PRINT("%s\n\n", "-REDUCE RULE-");

    StackCharItem top;
    GET_TOP_AND_POP(stack);

    // E -> i
    if (hasValue(top.value))
    {
        EXPECT_BREAK_AND_PUSH(stack, EXPR_E, top.type, top.optional, top.literal);
        
        return SYN_OK;
    }
    // E -> (E)
    else if (top.value == CLBR_E)
    {
        GET_TOP_AND_POP(stack);
        if (top.value == EXPR_E)
        {
            Data_type type = top.type;
            bool optional = top.optional;
            bool literal = top.literal;
            
            GET_TOP_AND_POP(stack);
            if (top.value == OPBR_E)
            {
                EXPECT_BREAK_AND_PUSH(stack, EXPR_E, type, optional, literal);

                return SYN_OK;
            }
        }

        return SYN_ERR;
    }
    // E -> E!
    else if (top.value == EXC_E)
    {
        GET_TOP_AND_POP(stack);
        if (top.value == EXPR_E)
        {
            EXPECT_BREAK_AND_PUSH(stack, EXPR_E, top.type, top.optional, top.literal);

            return SYN_OK;
        }
        
        return SYN_ERR;
    }
    // E -> E op E
    else if (top.value == EXPR_E)
    {
        Data_type type = top.type;
        bool optional = top.optional;
        bool literal = top.literal;

        GET_TOP_AND_POP(stack);
        top.type = type;
        top.optional = optional;
        top.literal = literal;
        int result = checkBinOperator(stack, top, gData);
        // If checkBinOperator failed
        if (result != SYN_OK)
            return result;

        generator_expr(gData->code,top.value);
        EXPECT_BREAK_AND_PUSH(stack, EXPR_E, type, optional, literal);

        return SYN_OK;
    }
    // No other rules.
    else
    {
        return SYN_ERR;
    }
}

// Main function of expression parser
int parseExpression(ParserDataT *gData, Token pushedTokens[], int tokensCnt, bool isArg)
{
    DEBUG_PRINT("%s\n", "PARSE EXPRESSION");
    DEBUG_PRINT("TOKENSCNT: %d\n", tokensCnt);
    DEBUG_PRINT("isArg: %d, followingEol: %d\n\n", isArg, gData->followingMlExprEOL);
    
    char operation;
    int result;
    bool end = false;
    bool firstIter = true;
    bool lastTokenFollowingEOL = false;
    bool inputLocked = false;
    Token inputToken;
    PrecSym input;
    Data_type type = TYPE_UNDEFINED;
    gData->result_type = TYPE_UNDEFINED;
    gData->CanConvertToDouble = true;
    bool optional = false;
    bool literal = false;
    bool firstOperand = true;
    StackCharItem *topTerminal;
    StackCharItem *top;

    StackChar *stack = malloc(sizeof(StackChar));
    if (StackChar_Init(stack) != STACK_OK)
        return INTERNAL_ERR;

    if (StackChar_Push(stack, DOLLAR_E, type, optional, literal) != STACK_OK)
        return INTERNAL_ERR;

    do
    {
        // If there are any pushed back tokens
        if (tokensCnt != 0)
            inputToken = pushedTokens[--tokensCnt];
        else
            inputToken = *(gData->token);
        
        if (!inputLocked)
        {
            input = getPrecSym(inputToken.type);
        }

        // Closing bracket for function arguments.
        if (isArg && input == CLBR_E)
        {
            input = DOLLAR_E;
            type = TYPE_UNDEFINED;
            optional = false;
            literal = false;
        }
        
        topTerminal = StackChar_TopTerminal(stack);
        top = StackChar_Top(stack);
    
        // End of the while
        if (firstIter == false && input == DOLLAR_E 
            && topTerminal->value == DOLLAR_E && top->value == EXPR_E)
        {
            DEBUG_PRINT("\n%s\n\n", "SUCCESS EXPR");
            gData->followingMlExprEOL = lastTokenFollowingEOL;
            end = true;
            FREE_STACK(stack);
            if(gData->argExpr == false)
             generator_save_exp(gData->code);
            gData->pushBack = true;

            return SYN_OK;
       }

        firstIter = false;

        DEBUG_PRINT("TOP_TERMINAL: %d, INPUT: %d\n", topTerminal->value, input);
        DEBUG_PRINT("TABLE INDEXES: TOP_TERMINAL: %d, INPUT: %d\n",
        getTableIndex(topTerminal->value), getTableIndex(input));

        operation = precTable[getTableIndex(topTerminal->value)]
                             [getTableIndex(input)];

        // To end multiline expressions
        if (operation == 'X' && lastTokenFollowingEOL)
        {
            input = DOLLAR_E;
            operation = precTable[getTableIndex(topTerminal->value)]
                                 [getTableIndex(input)];

            inputLocked = true;
            DEBUG_PRINT("%s\n", "LOCKING INPUT");
        }

        if (input == WHOLENUMBER_E)
        {
            type = TYPE_INT;
            optional = false;
            literal = true;
            if (firstOperand == true)
            {
                gData->result_type = type;
                firstOperand = false;
            }
        }
        else if (input == DECIMALNUMBER_E)
        {
            type = TYPE_DOUBLE;
            optional = false;
            literal = true;
            if (firstOperand == true)
            {
                gData->result_type = type;
                firstOperand = false;
            }
        }
        else if (input == STRINGVALUE_E)
        {
            type = TYPE_STRING;
            optional = false;
            literal = true;
            if (firstOperand == true)
            {
                gData->result_type = type;
                firstOperand = false;
            }
        }
        else if (input == ID_E)
        {
            avl_Data *variable = search_symbol(gData, inputToken.fluidPayload->payload);
            if (variable == NULL)
            {
                return SEM_UNDEF_VAR_ERR;
            }
            type = variable->type;
            optional = variable->optional;
            literal = false;
            if (variable->type == TYPE_INT)
            {
                gData->CanConvertToDouble = false;
            }
            if (firstOperand == true)
            {
                gData->result_type = type;
                firstOperand = false;
            }
        }
        else
        {
            type = TYPE_UNDEFINED;
            optional = false;
            literal = false;
        }

        DEBUG_PRINT("OPERATION: %c\n", operation);

        switch (operation)
        {
            // Equals
            case 'E':
                StackChar_Push(stack, input, type, optional, literal);

                lastTokenFollowingEOL = inputToken.followingEOL;
                if ((result = GetToken(gData->token)) != LEX_OK)
                {
                    FREE_STACK(stack);
                    generator_save_exp(gData->code);
                    return result;
                }

                break;
            // Push
            case 'P':
                StackChar_PushAfterTerminal(stack, BREAK_E, type, optional, literal);
                StackChar_Push(stack, input, type, optional, literal);

                
                    generator_push_something_to_stack(gData->code,inputToken);
                
                if (!firstIter)
                {
                    lastTokenFollowingEOL = inputToken.followingEOL;
                }

                if (tokensCnt == 0 && ((result = GetToken(gData->token)) != LEX_OK))
                {
                    FREE_STACK(stack);
                    generator_save_exp(gData->code);
                    return result;
                }

                break;
            // Reduce
            case 'R':
                result = reduceRule(stack,inputToken,gData);
                if (result != SYN_OK)
                {
                    FREE_STACK(stack);
                    return result;
                }

                break;
           default:
                FREE_STACK(stack);
                return SYN_ERR;
        }
    }
    while(!end);

    return SYN_OK;
}

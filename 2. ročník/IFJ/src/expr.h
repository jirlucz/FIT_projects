/**
 * @file expr.h
 *
 * @brief Header file for expression analysis.
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */


#ifndef EXPR_PARSER__
#define EXPR_PARSER__

#include "parser.h"
#include "stackChar.h"

//#include "scanner.h"

typedef enum
{
    PLUS_E=10,       // +  10 
    MIN_E,           // -  11  
    MUL_E,           // *  12
    DIV_E,           // /  13
    LESS_E,          // <  14
    MORE_E,          // >  15
    LEQ_E,           // <= 16
    MEQ_E,           // >= 17
    EQ_E,            // == 18
    NEQ_E,           // != 19
    OPBR_E,          // (  20
    CLBR_E,          // )  21
    ISNIL_E,         // ?? 22
    EXC_E,           // !  23
    ID_E,            // id 24
    WHOLENUMBER_E,   // INT 25
    DECIMALNUMBER_E, // DOUBLE 26
    STRINGVALUE_E,   // STRING 27
    DOLLAR_E,        // $  28
    EXPR_E,          // E  29
    BREAK_E          // < (when pushing to stack) 30
} PrecSym;

PrecSym getPrecSym(Token_Type type);
int getTableIndex(PrecSym sym);
int checkBinOperator(StackChar *stack, StackCharItem item, ParserDataT *gData);
int reduceRule(StackChar *stack,Token token,ParserDataT*gData);
int parseExpression(ParserDataT *gdata, Token pushedTokens[], int tokensCnt, bool isArg);

#endif

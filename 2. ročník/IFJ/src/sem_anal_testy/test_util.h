#ifndef SEM_ANAL_TEST_UTIL_H
#define SEM_ANAL_TEST_UTIL_H

#include "../symtable.h"
#include <stdio.h>

#define TEST(NAME, DESCRIPTION)                                                \
  void NAME() {                                                                \
    printf("[%s] %s\n", #NAME, DESCRIPTION);                                   \
    ParserDataT gData;                                                         \
    InitParserData(&gData);                                                    \

#define ENDTEST                                                                \
  printf("\n");                                                                \
  DisposeParserData(&gData);                                                   \
  }

typedef enum direction { left, right, none } direction_t;

typedef enum types
{
    //DEFAULT VALUE FOR FSM
    START = 1,

    //NUMBERS
    WHOLENUMBER= 100,   // 100 Whole number(6)
    DECIMALNUMBER,      // 101 Decimal number (6.7)
    //DECIMALEXP,         // 102 Decimal with exponent (6.7e-1)
    STRINGVALUE = 103,        // 103 String

    //KEYWORDS
    IF,                 // 104 IF keyword
    ELSE,               // 105 Else keyword
    FUNC,               // 106 Func keyword
    LET,                // 107 Let keyword
    VAR,                // 108 VAR keyword
    WHILE,              // 109 While keyword
    NIL,                // 110 Nil keyword
    RETURN,             // 111 Return keyword
    INTKEY,             // 112 Int keyword
    DOUBLEKEY,          // 113 Double keyword
    STRINGKEY,          // 114 String keyword

    //IDENTIFICATORS
    IDENTIFICATOR,      // 115 Name of variable or func 

    //LOGIC OPERATORS
    EQ,                 // 116 (=)
    EQEQ,               // 117 (==)
    NEQ,                // 118 (!=)
    LESSER,             // 119 (<) also spike open bracket !!! WARNING
    LESSEREQ,           // 120 (<=)
    GREATER,            // 121 (>) also closing spike bracket !!! WARNING
    GREATEREQ,          // 122 (>=)

    //BRACKETS
    OPBR,               // 123 <(>
    CLBR,               // 124 <)>
    OPCB,               // 125 <{>
    CLCB,               // 126 <}>

    //MATH OPERATORS
    MUL,                // 127 (*)
    DIV,                // 128 (/)
    PLUS,               // 129 (+)
    MINUS,              // 130 (-)

    //EOL, WWHITESPACE, EOFS
    EOL,                    // 131 EOL char
    WHITESPACE,             // 132 N-times whitespace
    EOFILE,                 // 133 End of file
    COMA,                   // 134 Coma (,)
    DOUBLEDOT,              // 135 (:)
    UNDERSCORE,             // 136 (_)
    LAMBDA,                 // 137 (->)
    QUESTIONMARK,           // 138 (?)
    DOUBLEQUESTIONMARK,     // 139 (??)
    EXCLAMATION,        // 140 (!)

    //NON-FINITE STATES
    _IDWOALFANUM           //141 (_ID) ID without alfanumeric char
}Token_Type;

typedef struct tokenStruct
{
    int unsigned id;
    char *identif;
    int value;
    double floatingValue;
    Token_Type type;

    bool followingEOL;
    bool followingEOF;
}Token;


void avl_print_subtree(avl_node_t *tree, char *prefix, direction_t from);
void avl_print_tree(avl_node_t *tree);
int GetToken(Token *token);
#endif

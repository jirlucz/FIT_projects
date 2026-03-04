#ifndef _SCANNER_H
#define _SCANNER_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "fluid_string.h"

/**
 * @file scanner.h
 * 
 *@brief Header file of scanner
 *
 * Project: IFJ 2023
 *
 *@authors Dominik Honza    <xhonza04@stud.fit.vutbr.cz>
 *@authors Honza Seibert    <xseibe00@stud.fit.vutbr.cz>
 *@authors Jiri Kotek       <xkotek09@stud.fit.vutbr.cz>
*/

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
    Fluids *fluidPayload;
    long long int value;
    double floatingValue;
    Token_Type type;

    bool followingEOL;
    bool followingEOF;
}Token;

/**
 * Function that sets input
 * 
 * @param file Pointer to a file
*/
void SetSourceFile(FILE *file);

/**
 * Function that initialize a token and add tokenID
 * 
 * @param token Pointer to a uninicialized token
 * @returns True if inicialization was succeful, False if token pointer was NULL or malloc fails
*/
bool InitToken(Token *token);

/**
 * Function that destroy a token
 * 
 * @param token Initialized (non NULL) pointer to token
 * @returns True if token was destroyed successfully 
*/
bool FlushToken(Token *token);

/**
 * Function adds a single char to token
 * @param token Initialized (non NULL) pointer to token
 * @param c Char to be loaded into FluidString
 * @returns True if load was successfull, False if token is NULL or char wasnt loaded
*/
bool AddCharToToken(Token *token,char c);

/**
 * Function adds type to token
 * @param token itialized (non NULL) pointer to token
*/
bool AddTypeToToken(Token *token,Token_Type type);

/**
 * Function will load a token with data from inputFile (must be set!)
 * 
 * @param token Pointer to a token
 * @returns int return value
*/
int GetToken(Token *token);

/**
 * Funcion moves pointer back to beggining of sourceFile
 * If sourceFile isnt set, does nothing
*/
void Return2Begg();

/**
 * Function checks if in array is stored symbol '"'
 * @param arr Pointer to array
 * @param size Size of array
 * @returns True if same, else False
*/
bool bingo (char *arr, int size);

/**
 * Function calculates offset inside multiline string and number of lines until reach """
 * @returns 0 if success, or non 0 if failed (-2 sourceFile == NULL, 99 reach EOF)
*/
int MultiLineOffset (int *resArr);

/**
 * Function converts hexadecimal string of 2 chars into ASCII char
 * 
 * @param HexaString Pointer to a string of max 2 chars (max value 255 - FF), if NULL returns 0
 * @returns char of ascii table
*/
int HexNumToChar(const char *HexaString);

/**
 * Function for processing NUMBERS
 * @param token
 * @returns int error value
*/
int processNumber (Token *token);

/**
 * Function will try to reach EOL 
*/
bool FollowingEOL ();

/**
 * Function will try to reach EOL 
*/
bool FollowingEOF ();

/**
 * Function removes comments if detected near actual token
 * @returns int error value
*/
int DeleteComments();

/**
 * Function sets FollowingEOL if EOL detected
 * @returns int error value
*/
int SetTokenFollow (Token *token);

/**
 * Function process escape secventions
 * @param token
 * @returns int value of escape secvention
*/
int EscapeSecventionProcessing (Token *token);

#endif


/**
 * @file stackChar.h
 *
 * @brief Defines StackChar
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */


#ifndef _STACKC_H_
#define _STACKC_H_

#include <stdbool.h>
#include "error.h"
#include "symtable.h"

typedef struct stackCharItem {
    int value;                     // Value of item.
    Data_type type;                // type of a variable
    bool optional;                 // Variable is optional
    bool literal;                  // Operand is a literal
    struct stackCharItem *next;    // Pointer to the next item.
} StackCharItem;

typedef struct {
    StackCharItem *top;
} StackChar;

int StackChar_Init( StackChar * );

bool StackChar_IsEmpty( const StackChar * );

StackCharItem *StackChar_Top( const StackChar *);

StackCharItem *StackChar_TopTerminal( const StackChar *);

void StackChar_Pop( StackChar * );

int StackChar_Push( StackChar *stack, int data, Data_type type, bool optional, bool literal);

int StackChar_PushAfterTerminal( StackChar *stack, int data, Data_type type, bool optional, bool literal);

void StackChar_Dispose( StackChar * );

#endif

/* stackChar.h */

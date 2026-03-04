/**
 * @file stack.h
 *
 * @brief Defines Stack
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */


#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include "error.h"
#include "scanner.h"

typedef struct {
    Token *array;
    int topIndex;
    int size;
} Stack;

int Stack_Init( Stack *, int );

bool Stack_IsEmpty( const Stack * );

bool Stack_IsFull( const Stack * );

int Stack_Top( const Stack *, Token * );

void Stack_Pop( Stack * );

int Stack_Push( Stack *, Token );

void Stack_Dispose( Stack * );

#endif

/* stack.h */

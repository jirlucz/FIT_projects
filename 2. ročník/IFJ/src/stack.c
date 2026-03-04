/**
 * @file stack.c.c
 *
 * @brief Implements functions of Stack
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */

#include <stdbool.h>
#include "stack.h"

int Stack_Init( Stack *stack, int stackSize) {
	if (stack == NULL)
		return INTERNAL_ERR;

	// char má velikost 1
    stack->size = stackSize;
	stack->array = malloc(stackSize);
	
	// pokud failne alokace
	if (stack->array == NULL)
		return INTERNAL_ERR;

	stack->topIndex = -1;

    return STACK_OK;
}

bool Stack_IsEmpty( const Stack *stack ) {
	return (stack->topIndex == -1);
}

bool Stack_IsFull( const Stack *stack ) {
	return (stack->topIndex == stack->size-1);
}

int Stack_Top( const Stack *stack, Token *dataPtr ) {
	if (Stack_IsEmpty(stack))
		return INTERNAL_ERR;

	// nastaví hodnotu ukazatele dataPtr na hodnotu prvku na vrcholu zasobníku
	*dataPtr = stack->array[stack->topIndex];

    return STACK_OK;
}

void Stack_Pop( Stack *stack ) {
	// Pokud je zásobník prázdný, vyskočí z funkce.
	if (Stack_IsEmpty(stack))
		return;

	--stack->topIndex;
}


int Stack_Push( Stack *stack, Token data ) {
	if (Stack_IsFull(stack))
		return INTERNAL_ERR;

	// Zvetší topIndex o 1 a na tento index vloží pushovaný prvek.
	stack->array[++stack->topIndex] = data;

    return STACK_OK;
}


void Stack_Dispose( Stack *stack ) {
	// Uvolní paměť, nastaví topIndex vzhledem k prázdnému poli a ukazatel na array nastaví na NULL.
	free(stack->array);
	stack->topIndex = -1;
	stack->array = NULL;
}

/* stack.c */

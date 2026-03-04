/**
 * @file stackChar.c
 *
 * @brief Implements functions of char Stack
 * 
 * Project: IFJ-2023
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */

#include <stdbool.h>
#include <stddef.h>
#include "stackChar.h"
#include "expr.h"

int StackChar_Init( StackChar *stack) {
	if (stack == NULL)
		return INTERNAL_ERR;

	stack->top = NULL;

    return STACK_OK;
}

bool StackChar_IsEmpty( const StackChar *stack ) {
	return (stack->top == NULL);
}

StackCharItem *StackChar_Top( const StackChar *stack ) {
	if (StackChar_IsEmpty(stack))
		return NULL;

    return stack->top;
}


StackCharItem *StackChar_TopTerminal( const StackChar *stack ) {
	if (StackChar_IsEmpty(stack))
		return NULL;

    for (StackCharItem *item = stack->top; item != NULL; item = item->next)
        if (item->value != BREAK_E && item->value != EXPR_E)
            return item;

    return NULL;
}

void StackChar_Pop( StackChar *stack ) {
    // Pokud je zásobník prázdný, vyskočí z funkce.
	if (StackChar_IsEmpty(stack))
		return;

	StackCharItem *item = stack->top;
    stack->top = item->next;

    free (item);
    item = NULL;
}

void StackChar_PopTimes( StackChar *stack, unsigned times) {
    while (times > 0)
    {
        StackChar_Pop(stack);
        times--;
    }
}


int StackChar_Push( StackChar *stack, int data, Data_type type, bool optional, bool literal) {
    StackCharItem *newItem = malloc(sizeof(StackCharItem));
    if (newItem == NULL)
        return INTERNAL_ERR;

    newItem->value = data;
    newItem->type = type;
    newItem->optional = optional;
    newItem->literal = literal;
    newItem->next = stack->top;

    stack->top = newItem;

    return STACK_OK;
}

int StackChar_PushAfterTerminal( StackChar *stack, int data, Data_type type, bool optional, bool literal) {
    StackCharItem *tmp = stack->top;
    StackCharItem *prevItem = NULL;

    while (tmp != NULL)
    {
        if (tmp->value != EXPR_E && tmp->value != BREAK_E)
        {
            StackCharItem *newItem = malloc(sizeof(StackCharItem));
            if (newItem == NULL)
                return INTERNAL_ERR;

            newItem->value = data;
            newItem->type = type;
            newItem->optional = optional;
            newItem->literal = literal;

            if (tmp == stack->top)
            {
                newItem->next = tmp;
                stack->top = newItem;
            }
            else
            {
                newItem->next = tmp;
                prevItem->next = newItem;
            }

            return STACK_OK;
        }

        prevItem = tmp;
        tmp = tmp->next;
    }

    return INTERNAL_ERR;
}


void StackChar_Dispose( StackChar *stack ) {
    while (!StackChar_IsEmpty(stack))
        StackChar_Pop(stack);
}

/* stackChar.c */

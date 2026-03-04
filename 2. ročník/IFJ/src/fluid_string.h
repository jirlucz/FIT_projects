#ifndef _FLUIDS_H
#define _FLUIDS_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"
#define MIN_ALLOC 4
/**
 * @file fluid_string.h
 * 
 * @brief Header file of fluid_string
 * 
 * Project: IFJ 2023
 * 
 * @authors Dominik Honza    <xhonza04@stud.fit.vutbr.cz>
 * @authors Honza Seibert    <xseibe00@stud.fit.vutbr.cz>
 * @authors Jiri Kotek       <xkotek09@stud.fit.vutbr.cz>

*/

typedef struct FluidString
{
   char *payload;
   unsigned int length;
   unsigned int actualLength;
}Fluids;

/**
*@brief Initialize fluid string
*@param FLUIDS * ptrFluidsElement
*@returns True if successfull else false
*/
Fluids* FluidInit(Fluids *ptrFLuidsElement);

/**
*@brief Frees fluid string element and deletes it
*@param FLUIDS * ptrFluidsElement
*@returns True if successfull else false
*/
bool FluidFlush(Fluids *ptrFluidsElement);

/**
*@brief Reallocs and enlarges field of fluid string
*@param FLUIDS * ptrFluidsElement
*@returns True if successfull else false
*/
bool FluidRealloc(Fluids *ptrFluidElement);

/**
*@brief Adds char if possible to fluid string
*@param FLUIDS * ptrFluidsElement
*@param c Char to add to string
*@returns True if successfull else false
*/
bool FluidAddChar(Fluids *ptrFluidElement,char c);

/**
*@brief Compares fluid string elemenet with another const string from keywords
*@param FLUIDS * ptrFluidsElement
*@param const char *keyword Keyword to compare the string with
*@returns True if same else false
*/
bool CmpFluidS(Fluids *ptrFluidElemnet,const char *keyword);


/**
*@brief Inserts a string into the Fluids structure
*@param Fluids *Element Pointer to the Fluids structure
*@param array String to be inserted
*@returns True if successful, else false
*/
bool FluidInsertString(Fluids* Element, char* array);

/**
*@brief Creates output from the Fluids structure
*@param Fluids *Element Pointer to the Fluids structure
*@returns True if successful, else false
*/
bool FluidCreateOutput(Fluids* Element, FILE* file);

/**
*@brief Inserts an integer into the Fluids structure
*@param Fluids *Element Pointer to the Fluids structure
*@param number Integer to be inserted
*@returns True if successful, else false
*/
bool FluidInsertInt(Fluids* Element, int number);
/**
*@brief Inserts an array into the Fluids structure
*@param Fluids *Element Pointer to the Fluids structure
*@param avl_data Container with payload to be inserted
*@returns True if successful, else false
*/
bool FluidInsertArray(Fluids*Element,avl_Data*node);

#endif

#include "fluid_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/**
 *@file fluid_string.c 
 * 
 *@brief    Implementation of fluid_string part of token
 *
 * Project: IFJ 2023
 * 
 *@authors  Dominik Honza    <xhonza04@stud.fit.vutbr.cz>
 *@authors  Honza Seibert    <xseibe00@stud.fit.vutbr.cz>
 *@authors  Jiri Kotek       <xkotek09@stud.fit.vutbr.cz>
*/

//Initialization of lfuid string
Fluids* FluidInit(Fluids*Element){
    Element->payload = (char*) malloc(MIN_ALLOC*sizeof(char));
        if( (Element->payload) != NULL)
        {
            Element->length = MIN_ALLOC;
            Element->actualLength = 0;
            return Element;
        }
        else
        {
            return NULL;
        }
}

// Frees element
bool FluidFlush(Fluids*Element){
    if(Element == NULL){
        return false;
    }
    else{
        free(Element->payload);
        Element = NULL;
        return true;
    }
}

//Reallocation of space for element if necessary
bool FluidRealloc(Fluids *Element){
    char *newPayload = (char*)realloc(Element->payload, sizeof(char) * (Element->length) * 2);

    if (newPayload != NULL) 
        {
        Element->payload = newPayload;
        Element->length = Element->length * 2;
        return true;
        }
    else
        return false;
}

// Adds individual char to payload if neccesary reallocs
bool FluidAddChar(Fluids *Element, char c){
    if(Element == NULL || Element->payload == NULL){
        return false;
    }

        if((Element->actualLength) == ((Element->length)-2)){
            FluidRealloc(Element);
        }

        Element->payload[Element->actualLength] = c;
        Element->payload[(Element->actualLength)+1] = '\0';
        Element->actualLength++;
        return true;
    
}

// Compares fluid payload with cons char array true if identical
bool CmpFluidS(Fluids*Element, const char* keyword){
    if (Element == NULL || keyword == NULL)
    {
        return false;
    }
    
    int diff = strcmp(Element->payload,keyword);
    if (diff == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

// Inserts char array into fluid payload
bool FluidInsertString(Fluids*Element,char*array){
    while(*array != '\0'){
        FluidAddChar(Element,*array);
        array++;
    }
    return true;
}

// Inserts char array into fluid payload
bool FluidInsertArray(Fluids*Element,avl_Data*node){
    int i = 0;
    
    while(node->identifier[i] != '\0' ){
        
        FluidAddChar(Element,node->identifier[i]);
        i++;
    }
    return true;
}

// Inserts int into fluid payload
bool FluidInsertInt(Fluids*Element,int number){
     char array[40];
     int cnt= 0;
     sprintf(array, "%d", number);
     
    while(array[cnt] != '\0'){
        FluidAddChar(Element,array[cnt]);
        cnt++;
    }
    return true;
}

bool FluidCreateOutput(Fluids*Element, FILE *file){
    // Write the string to the file using fputs
    if (fputs(Element->payload, file) == EOF) {
        return false;
    } 
       
    return true;
}


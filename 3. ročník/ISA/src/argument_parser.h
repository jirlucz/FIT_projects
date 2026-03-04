/**
 * File: argument_parser.h
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include "main.h"


/**
 * @brief Main function for parsing arguments
 * 
 * @param argc Number of arguments
 * @param argv Pointer to the array of arguments
 * @param argument_struct Pointer to a argumentStruct, where will be arguments stored
 * 
 * @return -1 if error occur or 0 if ok
 * 
 * @bug Function cant process "\" if its passed as argument in terminal
*/
int parse_argument(int argc, char* argv[], argumentStruct *argument_struct); 

/**
 * @brief Function prints help for the user with available program arguments
*/
void print_help_function();

/**
 * @brief Function fill argumentStruct with default value
 * 
 * @param argument_struct Pointer to a argumentStruct 
*/
void setup_argument_struct(argumentStruct *argument_struct);

/**
 * @brief Prints debug informations about given interface, arguments and names of files
 */
void print_debug(argumentStruct *argument_struct);

#endif /* ARGUMENT_PARSER_H */
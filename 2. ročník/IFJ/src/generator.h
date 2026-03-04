/**
 * @file generator.h
 * @brief Code generator interface.
 * @author Dominik Honza xhonza04
 */

#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <stdio.h>
#include <stdbool.h>
#include "fluid_string.h"
#include "expr.h"

/**
 * @brief Generates main scope and file headers with global variables.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_header(Fluids* code);

/**
 * @brief Generates inbuilt functions and main.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_start(Fluids* code);

/**
 * @brief Prints generated code to a file.
 * @param code A pointer to the Fluids structure containing the generated code.
 * @param file A pointer to the file where the code will be printed.
 */
void generator_print_to_file(Fluids* code, FILE* file);

/**
 * @brief Generates the main scope.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_main_start(Fluids* code);

/**
 * @brief Generates the end of the main scope.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_main_end(Fluids* code);

/**
 * @brief Generates a call to a label.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param id An integer representing the label identifier.
 */
void generator_func_call(Fluids* code, char* id);

/**
 * @brief Generates preparation for passing arguments to a function.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param id An integer representing the function identifier.
 */
void generator_pass_arguments_to_func(Fluids* code, char* id);

/**
 * @brief Generates a header with a name for a function.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param func_id An integer representing the function identifier.
 */
void generator_func_header(Fluids* code, char* func_id);

/**
 * @brief Generates a parameter with a name for a function.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param param_identificator An integer representing the parameter identifier.
 */
void generator_func_params(Fluids* code, char* param_identificator);

/**
 * @brief Generates parameter pops.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param param_identificator An integer representing the parameter identifier.
 */
void generator_func_pops(Fluids* code, char* param_identificator);

/**
 * @brief Generates the end of a function.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_func_end(Fluids* code,char*id);

/**
 * @brief Generates the header of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_if_header(Fluids* code);

/**
 * @brief Generates a jump to the else part of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param else_id An integer representing the else label identifier.
 */
void generator_if_jump_to_else(Fluids* code, int id);

/**
 * @brief Generates a jump to skip the else part of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param skip_id An integer representing the skip label identifier.
 */
void generator_if_jump_to_skip(Fluids* code, int id);

/**
 * @brief Generates a label for the else part of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param else_id An integer representing the else label identifier.
 */
void generator_if_label_else(Fluids* code, int id);

/**
 * @brief Generates a label to skip the else part of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param skip_id An integer representing the skip label identifier.
 */
void generator_if_label_skip(Fluids* code, int id);

/**
 * @brief Generates the header of a while loop.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_while_header(Fluids* code);

/**
 * @brief Generates the beginning of a while loop.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param while_id An integer representing the while label identifier.
 */
void generator_while_start(Fluids* code, int id);

/**
 * @brief Generates the end of a while loop.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param while_id An integer representing the while label identifier.
 */
void generator_while_label_skip(Fluids*code,int id);

/**
 * @brief Generates a jump to the while part of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param while_id An integer representing the while label identifier.
 */
void generator_while_jump_to_skip(Fluids* code, int id);

/**
 * @brief Generates a jump to the else part of an if statement.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param skip_id An integer representing the skip label identifier.
 */
void generator_while_jump_to_start(Fluids* code, int id);

/**
 * @brief Generates the definition of a variable.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param id An integer representing the variable identifier.
 */
void generator_var_define(Fluids* code,avl_Data*node);

/**
 * @brief Generates the assignment of a variable, goes in the stack until the expression is calculated, then writes.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param id An integer representing the variable identifier.
 */
void generator_var_asign(Fluids* code, avl_Data*node);

/**
 * @brief Adds a string to the Fluids element.
 * @param Element A pointer to the Fluids structure.
 * @param array A pointer to the array containing the string to be added.
 * @return True if successful.
 */
bool FluidInsertString(Fluids* Element, char* array);

/**
 * @brief Creates output for the Fluids element.
 * @param Element A pointer to the Fluids structure.
 * @return True if successful.
 */
bool FluidCreateOutput(Fluids* Element, FILE *file);

/**
 * @brief Prints the generated code.
 * @param code A pointer to the Fluids structure containing the generated code.
 */
void generator_print(Fluids* code);

/**
 * @brief Generates code for expressions.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param sym A PrecSym symbol representing the precedence of the expression.
 */
void generator_expr(Fluids* code, PrecSym sym);

/**
 * @brief Generates code to push something onto the stack.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param token A Token representing the item to be pushed onto the stack.
 */
void generator_push_something_to_stack(Fluids* code, Token token);

/**
 * @brief Generates code to obtain the value of a token.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param token A Token representing the item whose value needs to be obtained.
 */

/**
 * @brief Generates code to obtain the value of a token.
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param token A Token representing the item whose value needs to be obtained.
 */
void generator_value_of_token(Fluids* code, Token token);

/**
 * @brief Generates code to store the top of the stack.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_store_stack_top(Fluids* code);

/**
 * @brief Generates code to concatenate values on the stack.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_concat_on_stack(Fluids* code);

/**
 * @brief Generates code to convert the top of the stack to a double.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_convert_stack_env_to_double(Fluids* code);

/**
 * @brief Generates code to convert the second item on the stack to a double.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_convert_stack_env2_to_double(Fluids* code);

/**
 * @brief Generates code to convert the top of the stack to an integer.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_convert_stack_env_to_integer(Fluids* code);

/**
 * @brief Generates code to convert the second item on the stack to an integer.
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_convert_stack_env2_to_integer(Fluids* code);

/**
 * @brief Generates code to save stack top to GF@_exp
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_save_exp(Fluids*code);

/**
 * @brief Generates code to save stack top to GF@_if_while
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_while_save(Fluids*code);
/**
 * @brief Generates code to save result of while
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param *id A pointer to char array repsenting id
 */
void generator_var_asign_fluid(Fluids*code,char*id);
/**
 * @brief Generates code to save result of func
 * @param code A pointer to the Fluids structure for storing generated code.
 */
void generator_save_return(Fluids*code);
/**
 * @brief Generates code to write term
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param *token A pointer to token coamntiang term to write
 */
void generator_write(Fluids*code,Token*token);
/**
 * @brief Generates code to generate if let sattement
 * @param code A pointer to the Fluids structure for storing generated code.
 * @param *id Pointer to id of char array
 */
void generator_if_let(Fluids*code,char*id);

#endif

/**
 * @file error.h
 *
 * @brief Defines error codes for a compiler and semantic analysis.
 * 
 * Project: IFJ-2023
 *
 * This header file defines various error codes used by a compiler during
 * lexical, syntax, and semantic analysis. These error codes help identify
 * the type of error encountered during compilation.
 *
 * @author Jan Seibert <xseibe00@stud.fit.vutbr.cz>
 */

#ifndef ERROR_H__
#define ERROR_H__

#define LEX_ERR 1               // lexical analysis error
#define SYN_ERR 2               // syntax analysis error
#define SEM_UF_ERR 3            // undefined function, redefinition of variable
#define SEM_VAR_CNT_ERR 4       // incorrect function parameters or wrong return type of function
#define SEM_UNDEF_VAR_ERR 5     // usage of an undefined or uninitialized variable
#define SEM_FUN_RETURN_ERR 6    // missing or remaining in return command of function
#define SEM_TYPES_AR_ERR 7      // type compatibility in aritmetic, string or relational expressions
#define SEM_TYPES_UNSPEC_ERR 8  // unspecified type of variable
#define SEM_OTHER_ERR 9         // other semantic errors
#define INTERNAL_ERR 99         // internal failure of compiler

#define LEX_OK 11               // lexical analysis success
#define SYN_OK 12               // syntax analysis success
#define STACK_OK 13             // stack success

#endif

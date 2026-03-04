#include "../main.h"

#ifndef TCP_VARIANT_H__  
#define TCP_VARIANT_H__

/**
 * @brief                   Help Function for memory clear, called at end of the session
 * @param arguments         Pointer to structure of arguments
 * @param fileDescriptors   Pointer to structure of used file descriptors
 * 
*/
void tcp_cleanup(ProgramArguments *arguments, FileDescriprotStruct *fileDescriptors);


/**
 * @brief                   Help Function for printing help for user before /auth stage
*/
void printAUTHhelp ();


/**
 * @brief                   Function sets up all necessarilly things for tcp_run() function
 * @param arguments         Pointer to structure of used file descriptors
 * @param fileDescriptors   Pointer to structure of used file descriptors
 * 
 * @return 0 if success or -1 if error occurred
*/
int tcp_setup (ProgramArguments *arguments, FileDescriprotStruct *fileDescriptors);


/**
 * @brief                       Function sets up all necessarilly things for tcp_run() function
 * @param fileDescriptors       Pointer to structure of used file descriptors
 * @param userInputStruct       Pointer to structure, containg user provided informations
 * @param serverResponseStruct  Pointer to structure, containg server provided informations
 * @param arguments             Pointer to structure, containg programm informations
 * 
 * @return 0 if success or -1 if error occurred
*/
int tcp_run(FileDescriprotStruct *fileDescriptors, UserInput *userInputStruct, UserInput *serverResponseStruct, ProgramArguments *arguments);

#endif // TCP_VARIANT_H__


#include "../main.h"

#ifndef STDIN_PARSE_H__  
#define STDIN_PARSE_H__

/**
 * @brief               Function transform string into uppercase 
 * 
 * @param input         Pointer to a string to transform string into uppercase 
*/
void to_uppercase(char *input);

/**
 * @brief               Function transform uint16_t to string
 * 
 * @param number        uint16_t number
 * @param string        Pointer to a string of size 3 where will be stored string reprezentation of uint16_t
*/
void uint16_TO_string (uint16_t number, char *string);

/**
 * @brief               Function writes for the user of available programm command 
*/
void printHelpCommand ();


/**
 * @brief               Function calculates length of the string ending with /r/n
 * @attention               Function calculates '\0' as a part of the string, e.g. char string[] = \r\n\0, returns 3
 * 
 * @param string            -1 if error occurred or RN length of the string
 * 
 * @return 0 if success or -1 if error occurred
*/
int RN_strlen(char *string);


/**
 * @brief               Function builds a message in IPK24 CHAT protocol
 * 
 * @param variant           enum variant of the programm tcp/udp
 * @param userInputStruct   Pointer to structure where to store message for the server (userInputStruct->messageToBeSend)
 * 
 * @return                  0 if success or -1 if error occurred
*/
int build_message_for_server(enum programVariant variant, UserInput *userInputStruct);


/**
 * @brief               Function sends a message to the server
 * 
 * @param arguments         Struct of argument containing variant of the protocol
 * @param stringToSend      Pointer to a string which should be send
 * @param socket_FD         int of a file decriptor binded to the server
 * @param fileDescriptors   Pointer to structure where server adress, used only in UDP
 * @param userInputStruct   Pointer to structure where is stored length of the message, used only in UDP
 * 
 * @return                  0 if success or -1 if error occurred
*/
int send_message_to_server (ProgramArguments *arguments, char *stringToSend, int socket_FD, FileDescriprotStruct *fileDescriptors, UserInput *userInputStruct);


/**
 * @brief                   Function is used to process arguments launched with the programm
 * 
 * @param input                Input string fromm stdin
 * @param indexPosition        Position of index in input string (if you want to start from index 0 put -1) 
 * @param numberOfSpacesToSkip Skips all chars before this Nth space
 * @param maxCharToRead        Max chars to read
 * @param target               Destination where to store string
 * 
 * @returns last index in string that was processed
*/
int parse_single_inputpart (char input[], int indexPosition ,int numberOfSpacesToSkip, int maxCharToRead , char *target);


/**
 * @brief                   This function is used to break messages from the server to single parts separated SPACES
 * 
 * @param input                Input string
 * @param indexPosition        Position of index in input string (if you want to start from index 0 put -1)
 * @param maxIndexToRead       Maximum index to read/reach
 * @param target               Pointer to destination where to write a string
 * @param messageType          enum MessageType mainly for MessageContent 
 * 
 * @returns last index in string that was processed
*/
int parse_single_server_message_part (char input[], int indexPosition , int maxIndexToRead , char *target, enum messageType messageType);

/**
 *  @brief                   Function parse user provided input while network session
 * 
 * @param input                 Input string from stdin
 * @param userInputStruct       Pointer to a struct where stored user provided input
 * 
 * @returns 0 OK, -1 Error
*/
int user_input_parse(char input[], UserInput *userInputStruct);


/**
 *  @brief                   Function parse server provided messages while network session
 * 
 * @param recivedString              Input string from server
 * @param serverResponseStruct       Pointer to a struct where stored server provided input
 * 
 * @returns 0 OK, -1 Error
*/
int server_message_parse (char *recivedString,UserInput *serverResponseStruct);

#endif // STDIN_PARSE_H__



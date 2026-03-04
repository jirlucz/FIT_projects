#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netinet/udp.h> 
#include <netinet/ip.h> 
#include <arpa/inet.h>  
#include <netdb.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>



//Constant declaration
#define IPLEN       101
#define VARINATLEN  4
#define RAWINPUT    2000    //For direct reading stdin
#define MAXMESSLEN  1401
#define USERNAMELEN 21
#define SECRETLEN   129
#define COMMANDLEN  8
#define MAX_EVENTS  10

enum programVariant {
    tcp = 1,
    udp 
};

/**
 * @brief Struct for storing parameters
 * variant              -> udp/tcp              : -t
 * serverIP             -> max len 100          : -s
 * serverPort           -> uint16               : -p
 * udpTimeout           -> uint16               : -d
 * udpRetransmission    -> uint8                : -r
 * */
typedef struct {
    enum programVariant variant;
    char        serverIP[IPLEN];
    uint16_t    serverPort;
    double      udpTimeout;
    uint8_t     udpRetransmission;
    bool        help;

} ProgramArguments;

typedef struct {
    struct epoll_event event;
    struct sockaddr_in serverAddress;
    int socket_FD;  //SERVER
    int epoll_FD;   //STDIN
} FileDescriprotStruct;

typedef struct {
    uint16_t    messageID;
    bool        wasDelivered;
    uint8_t     NOofRetransmissions;
    struct timeval start_time;
} clientMessageConfirm;

enum userCommand {
    authCommand = 1,
    joinCommand,
    renameCommand,
    other,
    CommandError,
    helpCommand = 10 
};

enum messageType {
    ERR = 1,
    REPLY,
    NOTREPLY, //Unused
    AUTH,
    JOIN,
    MSG, 
    BYE,
    CONFIRM,
    OTHERmessageType
};

enum programState {
    startState = 1,
    authState,
    openState,
    endState,
    EOFstate,
    errorState = 99 
};



enum okeyNotOkey {
    NOK = 0,
    OK 
};

enum epollTriggeredBy {
    STDIN_Trigger = 1,
    SERVER_Trigger
};

/**
 * @brief Struct for storing user input from stdin
 *                  Max. len.
 * username             20
 * secret               128
 * displayName          20
 * message              1400
 * channelID            20
 * messageID            X
 * */
typedef struct {
    char        username[USERNAMELEN];
    char        secret[SECRETLEN];
    char        displayName[USERNAMELEN];
    char        message[RAWINPUT];
    char        messageToBeSend [RAWINPUT];
    char        channelID[USERNAMELEN];
    char        reply [2];
    int         messageLenght;      //Used in UDP variant
    uint16_t    messageID;          //Used in UDP variant
    uint16_t    Ref_messageID;      //Used in UDP variant
    clientMessageConfirm messageConfirm; //Used in UDP variant
    enum okeyNotOkey isOK;          //Used in TCP variant
    enum userCommand command;
    enum messageType messageType;

} UserInput;

#endif //main.h
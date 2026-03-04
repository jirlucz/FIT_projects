#include "../main.h"
//#include "../StdinParse/stdinparse.c"

void tcp_cleanup(ProgramArguments *arguments, FileDescriprotStruct *fileDescriptors){
    free(arguments);
    arguments = NULL;

    if (shutdown(fileDescriptors->socket_FD, SHUT_RDWR) == -1) 
    {
        fprintf(stderr, "ERR:  while closing TCP connection");
        close(fileDescriptors->socket_FD);
    }
}

void printAUTHhelp (){
    fprintf(stderr, "User is already authenticated\n");
    fprintf(stderr, "Try /help for more informations\n");
}

int tcp_setup (ProgramArguments *arguments, FileDescriprotStruct *fileDescriptors){

    //Creating a socket
    int family = AF_INET;
    int type = SOCK_STREAM;
    int socket_FD = socket(family, type, 0);
    if (socket_FD < 0)
    {
        fprintf(stderr, "ERR:  while creating a socket\n");
        return -1;
    }


    //Setting socket to nonblocking
    if (fcntl(socket_FD, F_SETFL, O_NONBLOCK) == -1) {
        fprintf(stderr, "ERR:  while  setting up a socket\n");
        return -1;
    }


    //Creating a sockaddr_in
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    //Converting a string format of IP address to bite format
    if (inet_pton(AF_INET, arguments->serverIP, &serverAddress.sin_addr) <= 0)
    {
        fprintf(stderr, "ERR:  while converting IP address to bite format\n");
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(arguments->serverPort);
    struct sockaddr *address = (struct sockaddr *) &serverAddress;
    int addressSize = sizeof(serverAddress);


    //Creating epoll instance
    struct epoll_event ev;
    int epoll_FD = epoll_create1(0);
    if (epoll_FD == -1) {
        fprintf(stderr,"ERR: epoll_create1 failed");
        return -1;
    }
    
    // Add STDIN_FILENO (standard input) to epoll monitoring
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epoll_FD, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        fprintf(stderr, "ERR:  epoll_ctl adding STDIN_FILENO in function TCP_SETUP\n");
        return -1;
    }


    // Add SOCKET_FD (server fd) to epoll monitoring
    ev.events = EPOLLIN; 
    ev.data.fd = socket_FD;
    if (epoll_ctl(epoll_FD, EPOLL_CTL_ADD, socket_FD, &ev) == -1) {
        fprintf(stderr, "ERR:  epoll_ctl adding socket_FD in function TCP_SETUP\n");
        return -1;
    }

    //Connect the client socket to server
    if (connect(socket_FD, address, addressSize) != 0)
    {
        if (errno != EINPROGRESS) 
        {
            fprintf(stderr, "ERR:  while establishing connection\n");
            return -1;
        }
                
    }
    
    fileDescriptors->socket_FD  =   socket_FD;
    fileDescriptors->epoll_FD   =   epoll_FD;
    fileDescriptors->event      =   ev;

    return 0;
}

int tcp_run(FileDescriprotStruct *fileDescriptors, UserInput *userInputStruct, UserInput *serverResponseStruct, ProgramArguments *arguments){
    char receivedData[RAWINPUT];                //2000
    char processedRecivedData [RAWINPUT];
    struct epoll_event events[MAX_EVENTS];
    int socket_FD = fileDescriptors->socket_FD;
    int epoll_FD = fileDescriptors->epoll_FD;
    enum programState actualProgrammState = startState;
    enum epollTriggeredBy messageFrom;
    bool isAuthenticated = false;
    bool isAuthenticatedTWICE = false;
    bool writeERRmessage = false;
    int num_events = 0;

    //Main infinite cycle
    while (1)
    {
        if (actualProgrammState == endState)
        {
            break;
        }
        
        num_events = epoll_wait(epoll_FD,events , MAX_EVENTS, -1);

        if (num_events == -1) {
            fprintf(stderr, "ERR: epoll_wait failed\n");
            return -1;
        }

        for (int i = 0; i < num_events; ++i)
        {
            //Reading from stdin
            if (events[i].data.fd == STDIN_FILENO)
            {
                messageFrom = STDIN_Trigger;
                if( fgets (userInputStruct->message, RAWINPUT-1, stdin) == NULL ) 
                {
                    // printf("FGETS GETS EOF\n");
                    actualProgrammState = EOFstate;
                    continue;
                }
                if (userInputStruct->message[0] == EOF)
                {
                    // printf("User insert Crtl D / EOF\n");
                    actualProgrammState = EOFstate;
                    continue;
                }
                
                user_input_parse(userInputStruct->message, userInputStruct);
                //printf("Napsana zprava je %s\n", userInputStruct->message);
                if (isAuthenticated == true && userInputStruct->command == authCommand)
                {
                    isAuthenticatedTWICE = true;
                }

                if (isAuthenticated == false && userInputStruct->command == authCommand)
                {
                    isAuthenticated = true;
                }

            }

            //Message from server arrived
            else if (events[i].data.fd == socket_FD)
            {
                messageFrom = SERVER_Trigger;
                int bytesRecived = recv(socket_FD, receivedData ,RAWINPUT+1, 0);
                if (bytesRecived == 0)
                {
                    fprintf(stderr, "ERR: Server closed the connection\n");
                    return 0;
                }
                else if (bytesRecived < 0)
                {
                    fprintf(stderr, "ERR: while reciving message from server\n");
                    return -1;
                }
                else
                {

                    //printf("Message from server is: %.*s\n", bytesRecived, receivedData);
                    memcpy(processedRecivedData, receivedData, bytesRecived);
                    processedRecivedData[bytesRecived] = '\0';
                    //printf("Processed server input: %s\n", processedRecivedData);
                    if (server_message_parse(processedRecivedData, serverResponseStruct) == -1)
                    {
                        writeERRmessage = true;
                    }
                    //In "startState" we are not accepting incomming messages
                    if (actualProgrammState == startState)
                    {
                        actualProgrammState = errorState;
                    }
                    
                }
            }
            else
            {
                fprintf(stderr, "ERR: event from unknown FD\n");
            }
            
            
        }

        //BREAK or CONTINUE commands
        //Now we check if user writes any local command as "/help" or "/rename"
        if (isAuthenticatedTWICE == true)
        {
            fprintf(stderr, "ERR: You cant authenticate more then once!\n");
        }
        
        if (userInputStruct->command == renameCommand || userInputStruct->command == CommandError || writeERRmessage == true)
        {
            writeERRmessage = false;
            continue;
        }
        else if (userInputStruct->command == helpCommand)
        {
            printHelpCommand();
            continue;
        }

        if (serverResponseStruct->messageType == ERR)
        {
            fprintf(stderr, "ERR FROM %s: %s", serverResponseStruct->displayName, serverResponseStruct->message);
            serverResponseStruct->messageType = OTHERmessageType;
            userInputStruct->messageType = BYE;
             actualProgrammState = endState;
                build_message_for_server(tcp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
            break;
        }
        
        
        //Now we check BYE Message in openState, we have to close session without any message
        if (actualProgrammState == openState && serverResponseStruct->messageType == BYE)
        {
            break;
        }
        if (userInputStruct->messageType == BYE)
        {
            break;
        }
        if (actualProgrammState == endState)
        {
            break;
        }
        if (actualProgrammState == EOFstate)
        {
            userInputStruct->messageType = BYE;
            build_message_for_server(tcp, userInputStruct);
            if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
            {
                fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                break;
            }
            break;
        }

        switch (actualProgrammState)
        {
        case startState:
            // printf("Start state\n");
            if (userInputStruct->command != authCommand)
            {
                fprintf(stderr, "ERR: Its requirement to authenticate, for more infro insert /help\n");
            }
            else
            {
                userInputStruct->messageType = AUTH;
                actualProgrammState = authState;
                build_message_for_server(tcp, userInputStruct);
                //printf("Zprava po buildu: %s", userInputStruct->messageToBeSend);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
            }
                break;

        case authState:
            // printf("Jsem ve stavu authState\n");
            if (serverResponseStruct->messageType == ERR)
            {
                fprintf(stderr, "ERR FROM %s: %s\n", serverResponseStruct->displayName, serverResponseStruct->message);
            }

            if (serverResponseStruct->isOK == OK)
            {
                actualProgrammState = openState;
                userInputStruct->command = other;
                fprintf(stderr, "Success: %s\n", serverResponseStruct->message);
                break;
            }
            else if (serverResponseStruct->isOK == NOK)
            {
                fprintf(stderr, "Failure: %s\n", serverResponseStruct->message);
                actualProgrammState = startState;
                break;
            }
            
            else //We recivid other message -> ErrorState -> BYE
            {
                //sending BYE message
                userInputStruct->messageType = BYE;
                build_message_for_server(tcp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
            }
            break;
        
        case openState:
            // printf("Jsem ve stavu OPEN\n");
            //Checks for auth command
            if (userInputStruct->command == authCommand)
            {
                printAUTHhelp ();
                break;
            }

            //We recived input from User on STDIN
            if (messageFrom == STDIN_Trigger)
            {
                if (userInputStruct->command == joinCommand)
                {
                    userInputStruct->messageType = JOIN;
                    build_message_for_server(tcp, userInputStruct);
                    if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                    {
                        fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                        break;
                    }
                    //HANDLE THE RESPONSE TODO
                }
                else if (userInputStruct->messageType == MSG)
                {
                    build_message_for_server(tcp, userInputStruct);
                    if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                    {
                        fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                        break;
                    }
                }
            }

            //We recived input from Server
            else
            {
                if (serverResponseStruct->messageType == MSG)
                {
                    if (server_message_parse(processedRecivedData, serverResponseStruct) == -1)
                    {
                        actualProgrammState = errorState;
                    }
                    else
                    {
                        fprintf(stdout, "%s: %s\n", serverResponseStruct->displayName ,serverResponseStruct->message); //MBY CHANGE TO userStruct diplayName
                    }
                }
                else if (serverResponseStruct->messageType == REPLY)
                {
                    if (serverResponseStruct->isOK == OK)
                    {
                        fprintf(stderr, "Success: %s\n", serverResponseStruct->message);
                    }
                    else    //serverResponseStruct->isOK == NOK
                    {
                        fprintf(stderr, "Failure: %s\n", serverResponseStruct->message);
                    }
                }
                //Ending the session
                else if (serverResponseStruct->messageType == BYE)
                {
                   ;
                   //Processed abowe in "BREAK or CONTINUE commands"
                }
                //We recived ERROR -> sending BYE
                else if (serverResponseStruct->messageType == ERR)
                {
                    fprintf(stderr, "ERR FROM %s: %s\n", serverResponseStruct->displayName, serverResponseStruct->message);
                    userInputStruct->messageType = BYE;
                    actualProgrammState = errorState;
                    build_message_for_server(tcp, userInputStruct);
                    if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                    {
                        fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                        break;
                    }
                }
                else if (serverResponseStruct->messageType == REPLY && userInputStruct->messageType == JOIN)
                {
                    if (serverResponseStruct->isOK == OK)
                    {
                        fprintf(stderr, "Success: %s\n", serverResponseStruct->message);
                    }
                    else    //serverResponseStruct->isOK == NOK
                    {
                        fprintf(stderr, "Failure: %s\n", serverResponseStruct->message);
                    }
                }
                else if (serverResponseStruct->messageType == JOIN)
                {
                    //fprintf(stdout, "Prisla mi JOIN zprava ze serveru\n");
                    fprintf(stdout, "Channel: %s, Username: %s\n", serverResponseStruct->channelID, serverResponseStruct->displayName);
                }
                else
                {
                    fprintf(stderr, "ERR: Unknown message recieved from the server\n");
                }
                
   
            }

            break;
        
        case endState:
            // printf("End state\n");
            if (userInputStruct->command == authCommand)
            {
                break;
            }
            userInputStruct->messageType = BYE;
             actualProgrammState = endState;
                build_message_for_server(tcp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
            break;
        
        case errorState:
            // printf("Error state\n");
            if (userInputStruct->command == authCommand)
            {
                printAUTHhelp ();
                break;
            }
             userInputStruct->messageType = ERR;
             actualProgrammState = endState;
             strcpy(userInputStruct->message, "In client occuerd error");
                build_message_for_server(tcp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, NULL, NULL) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
            break;
        
        default:
            break;
        }

        
    }

    return 0;
}
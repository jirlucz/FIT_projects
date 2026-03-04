#include "../main.h"

int udp_setup (ProgramArguments *arguments, FileDescriprotStruct *fileDescriptors){
    
    //Creating a socket
    int family = AF_INET;
    int type = SOCK_DGRAM;
    int socket_FD = socket(family, type, 0);
    if (socket_FD < 0)
    {
        fprintf(stderr, "ERR:  while creating a socket\n");
        return -1;
    }


    //Setting socket to nonblocking
    if (fcntl(socket_FD, F_SETFL, O_NONBLOCK) == -1) {
        fprintf(stderr, "ERR: while  setting up a socket\n");
        return -1;
    }


    //Creating a sockaddr_in
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    //Converting a string format of IP address to bite format
    if (inet_pton(AF_INET, arguments->serverIP, &serverAddress.sin_addr) <= 0)
    {
        fprintf(stderr, "ERR: while converting IP address to bite format\n");
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(arguments->serverPort);


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
        fprintf(stderr, "ERR: epoll_ctl adding socket_FD in function TCP_SETUP\n");
        return -1;
    }

    if (arguments->udpRetransmission < 0 || arguments->udpTimeout < 0)
    {
        fprintf(stderr, "ERR: udpRetransmission or udpTimeout cant be less then 0\n");
    }
    
    
    fileDescriptors->socket_FD  =   socket_FD;
    fileDescriptors->epoll_FD   =   epoll_FD;
    fileDescriptors->event      =   ev;
    fileDescriptors->serverAddress = serverAddress;

    return 0;
}


int udp_run (FileDescriprotStruct *fileDescriptors, UserInput *userInputStruct, UserInput *serverResponseStruct,ProgramArguments *arguments ){
    char receivedData           [RAWINPUT];               
    char processedRecivedData   [RAWINPUT];

    struct epoll_event events   [MAX_EVENTS];

    int socket_FD   =   fileDescriptors->socket_FD;
    int epoll_FD    =   fileDescriptors->epoll_FD;

    enum programState actualProgrammState = startState;

    socklen_t address_size = sizeof(fileDescriptors->serverAddress);

    enum epollTriggeredBy messageFrom;

    bool serverMessageIDalreadyDelivered = false;
    userInputStruct->messageConfirm.wasDelivered = true;

    bool isAuthenticated = false;
    bool isAuthenticatedTWICE = false;

    //Arrays where are stored delivered messagedID
    u_int16_t messIDrecivedFromTheServer [100];
    int messIDrecivedFromTheServerINDEX = 0;
    int timeoutMS = arguments->udpTimeout;

    // +1 bcs of initial
    //arguments->udpRetransmission++;
    userInputStruct->messageConfirm.NOofRetransmissions = arguments->udpRetransmission;

    int num_events = 0;

    //Variables for time measure
    struct timeval current_time;
    double elapsed_time = 0.0;

    //Set defaul value for messageID
    userInputStruct->messageID = 0;

    bool firstRun = true;

    while (1)
    {
        // sleep(1);
        //Start counting messageID from 0
        if (firstRun == true){
            firstRun = false;
        }
        else{
            userInputStruct->messageID++;
        }
        
        //Check for error state
        if (actualProgrammState == errorState)
        {
            break;
        }

        serverMessageIDalreadyDelivered = false;

        //We are expecting CONFIRM message
        if (userInputStruct->messageConfirm.wasDelivered == false)
        {
            num_events = epoll_wait(epoll_FD,events , MAX_EVENTS, timeoutMS);

            gettimeofday(&current_time, NULL);


            elapsed_time = (current_time.tv_sec - userInputStruct->messageConfirm.start_time.tv_sec) * 1000 + (current_time.tv_usec - userInputStruct->messageConfirm.start_time.tv_usec) /1000;
            // printf("elapsed time is: %f\n", elapsed_time);

        }
        //We are waiting for event
        else
        {
            // printf("Cekam na udalost\n");
            num_events = epoll_wait(epoll_FD,events , MAX_EVENTS, -1);
        }
        
        
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
                    actualProgrammState = EOFstate;
                    continue;
                }
                user_input_parse(userInputStruct->message, userInputStruct);
                //Bahavior depend on actual state, processed later
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
                int bytesRecived = recvfrom(socket_FD, receivedData ,RAWINPUT+1, 0,(struct sockaddr *)&(fileDescriptors->serverAddress), &address_size);
                if (bytesRecived == 0)  //Shouldnt occur
                {
                    fprintf(stderr, "ERR: Server closed the connection\n");
                    return 0;
                }
                else if (bytesRecived < 0)
                {
                    fprintf(stderr, "ERR: Error while reciving message from server\n");
                    return -1;
                }
                else
                {
                    if (server_datagram_parse(receivedData, serverResponseStruct) != 0)
                    {
                        return -1;
                    }
                    //Message arrived, check if CONFIRM message is necessarily to send
                    if (serverResponseStruct->messageType != CONFIRM)
                    {
                        // printf("Sending CONFIRM\n");
                        userInputStruct->messageType = CONFIRM;
                        userInputStruct->Ref_messageID = serverResponseStruct->messageID;   //CHECK
                        build_message_for_server(udp, userInputStruct);
                        if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                        {
                            fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                            break;
                        }
                        gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
                        //Store MessID in array
                        serverMessageIDalreadyDelivered = message_was_already_recived(messIDrecivedFromTheServer, messIDrecivedFromTheServerINDEX, serverResponseStruct->messageID );
                        if (serverMessageIDalreadyDelivered == false)
                        {
                            messIDrecivedFromTheServerINDEX = store_message_id (messIDrecivedFromTheServer, messIDrecivedFromTheServerINDEX, serverResponseStruct->messageID);      
                        }

                        continue;
                    }
                    //serverResponseStruct->messageType == CONFIRM, We have to check if we get CONFIRM in time or epoll timeouted
                    else
                    {
                        // printf("Dosel CONFIRM\n");
                        // printf("ID zaslane zpravy: %u\n", userInputStruct->messageConfirm.messageID);
                        // printf("Povrzovaci ID zpravy ze serveru %d\n", serverResponseStruct->Ref_messageID );
                        if (userInputStruct->messageConfirm.messageID == serverResponseStruct->Ref_messageID && elapsed_time < timeoutMS)
                        {
                            //Reset to def values
                            // printf("Nastavuji na TRUE\n");
                            userInputStruct->messageConfirm.wasDelivered = true;
                            userInputStruct->messageConfirm.NOofRetransmissions = arguments->udpRetransmission;
                        }
                        else
                        {
                            // printf("zbyvajici pocet pokusu na doruceni %d\n", userInputStruct->messageConfirm.NOofRetransmissions);
                            if (userInputStruct->messageConfirm.NOofRetransmissions == 0)
                            {
                                // WHAT TO DO?
                                printf("ERR: message was unsuccefully delivered\n");
                                userInputStruct->messageConfirm.wasDelivered = true;
                                continue;
                            }
                            
                            userInputStruct->messageConfirm.NOofRetransmissions--;
                            //SEND AGAIN
                            if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                            {
                                fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                                break;
                            }
                            gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
                        }
                    }
                }
            }
            else
            {
                fprintf(stderr, "ERR: event from unknown FD\n");
            }
        }

        // int port = ntohs(fileDescriptors->serverAddress.sin_port);
        // printf("Aktualni port v programu je %d\n", port);


        // *** TMP BLOCK OF CATCHING CONFIRM MSGs *** //
        if (serverResponseStruct->messageType == CONFIRM)
        {
            continue;
        }
        

        // *** END OF TMP BLOCK OF CATCHING CONFIRM MSGs *** //


        // ***  OF BREAK or CONTINUE COMMANDS *** //
        //If we recived message that was already delivered, do nothing and continue
        if (isAuthenticatedTWICE == true)
        {
            fprintf(stderr, "ERR: You cant authenticate more then once!\n");
        }

        if (serverMessageIDalreadyDelivered == true)
        {
            continue;
        }
        if (serverResponseStruct->messageType == ERR)
        {
            fprintf(stderr, "ERR FROM %s: %s", serverResponseStruct->displayName, serverResponseStruct->message);
            serverResponseStruct->messageType = OTHERmessageType;
            break;
        }
        
        //Now we check if user writes any local command as "/help" or "/rename"
        if (userInputStruct->command == renameCommand || userInputStruct->command == CommandError)
        {
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
                build_message_for_server(udp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
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
            build_message_for_server(udp, userInputStruct);
            if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
            {
                fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                break;
            }
            gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
            break;
        }
        // *** END OF BREAK or CONTINUE COMMANDS *** //

        

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
                build_message_for_server(udp, userInputStruct);
                //printf("Zprava po buildu: %s", userInputStruct->messageToBeSend);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
                gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
            }
                break;

        case authState:
            if (serverResponseStruct->messageType == ERR)
            {
                fprintf(stderr, "ERR FROM %s: %s\n", serverResponseStruct->displayName, serverResponseStruct->message);
            }
            // printf("Jsem ve stavu authState\n");
            if (serverResponseStruct->reply[0] == '1')
            {
                actualProgrammState = openState;
                userInputStruct->command = other;
                fprintf(stderr, "Success: %s\n", serverResponseStruct->message);
                break;
            }
            else if (serverResponseStruct->reply[0] == '1')
            {
                fprintf(stderr, "Failure: %s\n", serverResponseStruct->message);
                actualProgrammState = startState;
                break;
            }
            else //We recivid other message -> ErrorState -> BYE
            {
                //sending BYE message
                userInputStruct->messageType = BYE;
                build_message_for_server(udp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
                gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
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
                    build_message_for_server(udp, userInputStruct);
                    if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                    {
                        fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                        break;
                    }
                    gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
                    //HANDLE THE RESPONSE TODO
                }
                else if (userInputStruct->messageType == MSG)
                {
                    build_message_for_server(udp, userInputStruct);
                    if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                    {
                        fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                        break;
                    }
                    gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
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
                    fprintf(stdout, "%s: %s\n", serverResponseStruct->displayName ,serverResponseStruct->message); //MBY CHANGE TO userStruct diplayName
                }
                else if (serverResponseStruct->messageType == REPLY)
                {
                    if (serverResponseStruct->reply[0] == '1')
                    {
                        fprintf(stderr, "Success: %s\n", serverResponseStruct->message);
                    }
                    else    //serverResponseStruct->reply[0] == '0'
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
                    build_message_for_server(udp, userInputStruct);
                    if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                    {
                        fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                        break;
                    }
                    gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);
                }
                else if (serverResponseStruct->messageType == REPLY && userInputStruct->messageType == JOIN)
                {
                    if (serverResponseStruct->reply[0] == '1')
                    {
                        fprintf(stderr, "Success: %s\n", serverResponseStruct->message);
                    }
                    else    //serverResponseStruct-->reply[0] == '0'
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
                    fprintf(stderr, "ERR: unknown message from the server\n");
                    // printf("Enum messageType %d\n",serverResponseStruct->messageType );
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
                build_message_for_server(udp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
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
                build_message_for_server(udp, userInputStruct);
                if (send_message_to_server(arguments, userInputStruct->messageToBeSend, socket_FD, fileDescriptors, userInputStruct) < 0)
                {
                    fprintf(stderr, "ERR: In TCP_RUN function occurs error while sending message to the server\n");
                    break;
                }
                gettimeofday(&userInputStruct->messageConfirm.start_time, NULL);

            break;
        
        default:
            break;
        }
        
    }

    return 0;
}
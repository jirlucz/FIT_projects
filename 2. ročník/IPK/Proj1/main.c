#include "main.h"
#include "ArgumentParse/argumentParse.c"
#include "StdinParse/stdinparse.c"
#include "Tcp/tcpvariant.c"
#include "Udp/udpvariant.c"

//Global pointer to argument due to free
ProgramArguments *arguments = NULL;
UserInput userInputStruct;
int socket_FD = -1;
int epoll_FD = -1;

void sigintHandler(int sig_num) {
    //printf("\nCtrl+C pressed. Exiting...\n");
    //TODO BYE mess UDP wait?
    
    userInputStruct.messageType = BYE;
    if (arguments == NULL)
    {
        exit(0);
    }
    

    if (arguments->variant == tcp)
    {
        build_message_for_server(tcp, &userInputStruct);
        if (send_message_to_server(arguments, userInputStruct.messageToBeSend, socket_FD, NULL, NULL) < 0)
        {
            fprintf(stderr, "ERR: Sending BYE message in TCP variant was unsuccessful\n");
        }

        if (shutdown(socket_FD, SHUT_RDWR) == -1) {
            fprintf(stderr, "Error while closing TCP connection\n");
            //close(socket_FD);
        }
    }
    else
    {
        build_message_for_server(udp, &userInputStruct);
        if (send_message_to_server(arguments, userInputStruct.messageToBeSend, socket_FD, NULL, NULL) < 0)
        {
            fprintf(stderr, "ERR: Sending BYE message in UDP variant was unsuccessful\n\n");
        }
    }

    if (arguments != NULL)
    {
        free(arguments);
        arguments = NULL;
    }
    
    close(socket_FD);  
    close(epoll_FD); 
    socket_FD = -1;
    epoll_FD = -1;
    exit(0);
}

int main (int argc,char* argv[]){

    //Handling Crtl^C command
    signal(SIGINT, sigintHandler);

    int setupReturn = -1;

    //Argument parse
    arguments = Argument_Parse(argc, argv);
        if (arguments == NULL)
        {
            fprintf(stderr, "Error while processing arguments called with programm\n");
            return -1;
        }

        if (arguments->help == true)
        {
            return 0;
        }

    //Debug print
    // printf("Varianta: %s\n", argv[2]);
    // printf("Server IP: %s\n", arguments->serverIP);
    // printf("Server port: %u\n", arguments->serverPort);
    // printf("UDP retransmission: %u\n", arguments->udpRetransmission);
    // printf("UDP Timeout: %f\n", arguments->udpTimeout);


    //Setup
    FileDescriprotStruct fileDescriptors;
    UserInput serverResponseStruct;
    //Calling specific variant 
    if (arguments->variant == tcp)
    {
        if (tcp_setup(arguments, &fileDescriptors) == -1)
        {
             fprintf(stderr, "Error while setuping tcp");
             return -1;
        }
        socket_FD = fileDescriptors.socket_FD;
        epoll_FD = fileDescriptors.epoll_FD;
        tcp_run(&fileDescriptors, &userInputStruct, &serverResponseStruct, arguments);
        tcp_cleanup(arguments, &fileDescriptors);
    }
    else if (arguments->variant == udp)
    {
        setupReturn = udp_setup(arguments, &fileDescriptors);
        if (setupReturn == -1)
        {
             fprintf(stderr, "Error while setuping udp");
             return -1;
        }
        socket_FD = fileDescriptors.socket_FD;
        udp_run(&fileDescriptors, &userInputStruct, &serverResponseStruct, arguments);
    }

    else
    {
        fprintf(stderr, "Wrong variant choose tcp/udp");
        if (arguments != NULL)
        {
            free(arguments);
            arguments = NULL;
        }
        return -1;
    }
    

    if (socket_FD != -1) {
        close(socket_FD);
        socket_FD = -1;
    }

    if (epoll_FD != -1) {
        close(epoll_FD);
        epoll_FD = -1;
    }

    if (arguments != NULL)
    {
        free(arguments);
        arguments = NULL;
    }
    


    return 0;
} 
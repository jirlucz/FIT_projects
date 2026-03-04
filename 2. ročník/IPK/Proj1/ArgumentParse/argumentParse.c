#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>


#include "../main.h"

#define NDEBUG

bool isIPaddress (char *input){
    struct sockaddr_in sa;
    int value = inet_pton(AF_INET, input, &(sa.sin_addr));

    if (value == 1)
    {
        //Is IP adress
        return true;
    }
    else
    {
        //Isnt IP adress
        return false;
    }
    
}

int copyString(char *source, char *destination, int maxLen) {
    size_t length = strlen(source) + 1; //+1 due to '\0'
        if (length > (size_t)maxLen)
        {
            fprintf(stderr, "Length of argument are too high\n");
            return -1;
        }
        

    memcpy(destination, source, length);

    return 0;
}

uint16_t Convert_To_uint16_t (char *source){
    char *endptr = NULL;
    unsigned long uint16Value = strtoul(source, &endptr, 10);

    assert(*endptr == '\0');
    assert((uint16_t)uint16Value <= UINT16_MAX);

    return (uint16_t)uint16Value;
}

uint8_t Convert_To_uint8_t (char *source){
    char *endptr = NULL;

    unsigned long uint8Value = strtoul(source, &endptr, 10);

    assert(*endptr == '\0');
    assert((uint8_t)uint8Value <= UINT8_MAX);

    return  (uint8_t)uint8Value;
}

ProgramArguments *Argument_Parse(int argc, char* argv[]){
    ProgramArguments *arguments = malloc(sizeof(ProgramArguments));
        if (arguments == NULL)
        {
            return NULL;
        }

    //Setup
    arguments->serverPort = 4567;   //Default value
    arguments->udpRetransmission = 3;
    arguments->udpTimeout = 250;
    arguments->help = false;

    char    variantString[VARINATLEN];
    //printf("Pocet argumetu je: %d\n", argc);

    for (int i = 1; i < argc; i++)
    {   
        //printf("%s\n", argv[i]);
        if (argv[i][0] == '-')    
        {
            switch (argv[i][1])
            {
            case 't':
                if (copyString(argv[i+1], variantString, VARINATLEN) == -1)
                {
                    free(arguments);
                    return NULL;
                }
                i++;
                continue;
            case 's':
                if (copyString(argv[i+1], arguments->serverIP, IPLEN) == -1)
                {
                    free(arguments);
                    return NULL;
                }
                i++;
                continue;
            case 'p':
                arguments->serverPort = Convert_To_uint16_t(argv[i+1]);
                i++;
                continue;
            case 'd':
                arguments->udpTimeout = Convert_To_uint16_t(argv[i+1]);
                i++;
                continue;
            case 'r':
                arguments->udpRetransmission = Convert_To_uint8_t(argv[i+1]);
                i++;
                continue;
            case 'h':
                printf("To run this program use there parameters: \n");
                printf("* variant              -> udp/tcp              : -t\n");
                printf("* serverIP             -> max len 100          : -s\n");
                printf("* serverPort           -> uint16               : -p\n");
                printf("* udpTimeout           -> uint16               : -d\n");
                printf("* udpRetransmission    -> uint8                : -r\n");
                arguments->help = true;
                break;
            
            
            default:
                return NULL;
            }
        }
        
    }
    
    if (isIPaddress(arguments->serverIP) == false)
    {
        struct addrinfo *addrInfoStruct;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET; // IPv4
            hints.ai_socktype = SOCK_STREAM; // TCP socket
        int getAddrValue = getaddrinfo(arguments->serverIP, NULL, &hints, &addrInfoStruct);

        if (getAddrValue != 0)
        {
            fprintf(stderr, "Error while DNS lookup at getaddinfo\n");
            return NULL;
        }



         char ipstr[INET_ADDRSTRLEN];
        struct addrinfo *p;
        
        //For printing all available IP addrs comment "p = addrInfoStruct;" and uncomment for cycle
            p = addrInfoStruct;
        //for (p = addrInfoStruct; p != NULL; p = p->ai_next){
            void *addr;
            char *ipver;

            // Get the pointer to the address itself:
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";

            // Convert the IP to a string and print it:
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            strcpy(arguments->serverIP, ipstr);
           if (addr == NULL)    //Change == -> !-
           {
                 fprintf(stderr,"Error in fucntion isIPaddress %s: %s\n", ipver, ipstr);
           }
           
        //}
        
        freeaddrinfo(addrInfoStruct); // Free the linked list
        

    }
    
    if (variantString[0] == 'u')
    {
        arguments->variant = udp;
    }
    else
    {
        arguments->variant = tcp;
    }
    


    
        
    return arguments;
} 
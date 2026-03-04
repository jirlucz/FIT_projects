#include "stdinparse.h"
#include "../main.h"

void to_uppercase(char *input){

    if (input == NULL)
    {
        fprintf(stderr, "ERR: Function to_uppercase recived NULL pointer\n");
    }
    
    int len = strlen(input);

    //Choose max
    if (len < RN_strlen(input))
    {
        len = RN_strlen(input);
    }
    
    for (int i = 0; i < len; i++)
    {
        if (input[i] == '\0')
        {
            break;
        }
        else if (input[i] >= 97 && input[i] <= 122)
        {
            input[i] = input[i]-32;
        }
    }

    return;
}

void uint16_TO_string (uint16_t number, char *string){
    //Transfer to network byte order
    uint16_t networkOrder = htons(number);

    // Create a 2-byte string (char array)
    string[0] = networkOrder ; 
    string[1] = networkOrder >> 8;   

    string[2] = '\0';

    
}

uint16_t string_TO_uint16 (uint16_t number, char *string){
    //Transfer to network byte order
    uint16_t result = atoi(string);
    return result;
}

void int_TO_string (int number, char *string){
    snprintf(string, 2, "%d", number); 
    string[1] = '\0';

}

int index_string_concat (int const startIndex, char  *destination, char const *source, bool putNULL, bool boolID){
    int sourceLen = strlen(source);

    if (boolID == true)
    {
        sourceLen = 1;
    }

    int sourceIndex = 0;
    int backupIndex = startIndex;   //To use i after for


    for (int i = startIndex; i < (sourceLen+startIndex); i++)
    {
        // printf("Zapisuji: %02X\n", source[sourceIndex]);
        destination[i] = source[sourceIndex];
        sourceIndex++;
        backupIndex ++;
    }

    if (putNULL == true)
    {
        destination[backupIndex] = '\0';
        // printf("Zapisuji: %02X\n", destination[backupIndex]);
        backupIndex++;
    }
    
    
    return backupIndex;
}

void printHelpCommand (){
    printf("Available options: \n");
    printf("\t/auth {Username} {Secret} {DisplayName} \n");
        printf("\t\tSends AUTH message with the data provided from the command to the server (and correctly handles the Reply message), locally sets the DisplayName value (same as the /rename command)\n");
    printf("\t/join {ChannelID}\n");
        printf("\t\tSends JOIN message with channel name from the command to the server (and correctly handles the Reply message)\n");
    printf("\t/rename {DisplayName}\n");
        printf("\t\tLocally changes the display name of the user to be sent with new messages/selected commands\n");
    return;
}

int RN_strlen(char *string){
  
    if (string == NULL)
    {
        fprintf(stderr, "ERR: RN_strlen gets NULL pointer\n");
        return -1;
    }
    
    char RNend[] = "\r\n";

    char *result = strstr(string, RNend);

    if (result != NULL) {
        //printf("Substring found at index: %ld\n", result - string);
        return (result - string + 3);   //We can substract two pointers +3 bcs we have three chars extra (behind of index) 
    } else {
        // fprintf(stderr, "ERR: substring /r/n not found.\n");
        return -1;
    }
    
}

int send_message_to_server (ProgramArguments *arguments, char *stringToSend, int socket_FD, FileDescriprotStruct *fileDescriptors, UserInput *userInputStruct){

    int bytes_tx;
    switch (arguments->variant)
    {
    //  CASE TCP
    case tcp:

        bytes_tx = send(socket_FD, stringToSend, RN_strlen(stringToSend)-1, 0);
        if (bytes_tx != RN_strlen(stringToSend)-1 || bytes_tx < 0)
        {
             fprintf(stderr, "ERR: TCP packet transfer was unsuccessful\n");
                 return -1;
        }
        
        return 0;
        break;
    //  END CASE TCP

    //  CASE UDP    
    case udp:
        if (fileDescriptors == NULL)
        {
            fprintf(stderr, "ERR: function send_message_to_server recived NULL pointer\n");
            return -1;
        }
        
        bytes_tx = sendto(socket_FD, (const char *)stringToSend, userInputStruct->messageLenght, 0,(const struct sockaddr *) &fileDescriptors->serverAddress, sizeof(fileDescriptors->serverAddress));
        userInputStruct->messageConfirm.messageID = userInputStruct->messageID;
        userInputStruct->messageConfirm.wasDelivered = false; 

        if (bytes_tx != userInputStruct->messageLenght)
        {
            fprintf(stderr, "ERR: while sending a UDP packet in fuction send_message_to_server()\n");
            return -1;
        }


        return 0;
        break;
    //END CASE UDP

    default: 
        return -1;
        break;
    } 
    
    
    
}

int build_message_for_server(enum programVariant variant, UserInput *userInputStruct){

    if (userInputStruct == NULL)
    {
        fprintf(stderr, "ERR: Function build_message_for_server gets NULL pointer argument\n");
        return -1;
    }
    
    //Clearing the output string
    userInputStruct->messageToBeSend[0] = '\0';
    //Make sure the string has end
    userInputStruct->messageToBeSend[RAWINPUT-1] = '\0';

    char uint16String [5];
    char REFuint16String [5];
    char header[2];
        header [1] = '\0';
    int lastIndexModification = 0;


    //Cut \n symbol from STDIN
    char *newLineChatPosition =strstr(userInputStruct->message, "\n");
        if (newLineChatPosition != NULL)
        {
            *newLineChatPosition = '\0';
        }


    //Variant specific building messages
    // *** TCP VARIANT *** //
    if (variant == tcp)
    {
        switch (userInputStruct->messageType)
        {
        case AUTH:
            strcat(userInputStruct->messageToBeSend, "AUTH ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->username);
            strcat(userInputStruct->messageToBeSend, " AS ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->displayName);
            strcat(userInputStruct->messageToBeSend, " USING ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->secret);
            strcat(userInputStruct->messageToBeSend, "\r\n");
            break;
        case JOIN:
            strcat(userInputStruct->messageToBeSend, "JOIN ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->channelID);
            strcat(userInputStruct->messageToBeSend, " AS ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->displayName);
            strcat(userInputStruct->messageToBeSend, "\r\n");
            break;
        case ERR:
            strcat(userInputStruct->messageToBeSend, "ERR");
            strcat(userInputStruct->messageToBeSend, " FROM ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->displayName);
            strcat(userInputStruct->messageToBeSend, " IS ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->message);
            strcat(userInputStruct->messageToBeSend, "\r\n");
            break;
        case REPLY:
            strcat(userInputStruct->messageToBeSend, "REPLY");
            strcat(userInputStruct->messageToBeSend, " OK ");
            strcat(userInputStruct->messageToBeSend, " IS ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->message);
            strcat(userInputStruct->messageToBeSend, "\r\n");
            break;
        case NOTREPLY:
            strcat(userInputStruct->messageToBeSend, "REPLY");
            strcat(userInputStruct->messageToBeSend, " NOK ");
            strcat(userInputStruct->messageToBeSend, " IS ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->message);
            strcat(userInputStruct->messageToBeSend, "\r\n");
            break;
        case MSG:
            strcat(userInputStruct->messageToBeSend, "MSG");
            strcat(userInputStruct->messageToBeSend, " FROM ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->displayName);
            strcat(userInputStruct->messageToBeSend, " IS ");
            strcat(userInputStruct->messageToBeSend, userInputStruct->message);
            strcat(userInputStruct->messageToBeSend, "\r\n");
            break;
        case BYE:
            strcat(userInputStruct->messageToBeSend, "BYE\r\n");
            break;
        default:
            fprintf(stderr, "ERR: while building message for server in function build_message_for_server\n");
            return -1;
        }

        return 0;
    }

    // *** UDP VARIANT *** //
    else if (variant == udp)
    {
        uint16_TO_string(userInputStruct->messageID,        uint16String);
        uint16_TO_string(userInputStruct->Ref_messageID,    REFuint16String);
        switch (userInputStruct->messageType)
        {
        case AUTH:
            header[0] = '\x02'; 
            
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[1], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->username, true, false);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->displayName, true, false);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->secret, true, false);
            break;

        case JOIN:
            header[0] = '\x03'; 
            
            //Header
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            //MessID
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[1], false, true);
            //Channel ID
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->channelID, true, false);
            //Display Name
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->displayName, true, false);
            break;

        case ERR:
            header[0] = '\xFE'; 
            
            //Header
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            //Message ID
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[1], false, true);
            //Display Name
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->displayName, true, false);
            //Message Content
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->message, true, false);
            break;

        case REPLY:
            header[0] = '\x01'; 
            
            //Header
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            //MessID
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[1], false, true);
            //Result
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->reply, false, false);
            //REFMessID
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &REFuint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &REFuint16String[1], false, true);
            //MessageContent
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->message, true, false);
            break;

        case NOTREPLY:
            break;

        case MSG:
            header[0] = '\x04'; 
            
            //Header
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            //Message ID
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[1], false, true);
            //Display Name
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->displayName, true, false);
            //Message Content
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, userInputStruct->displayName, true, false);
            break;

        case BYE:
            header[0] = '\x0FF'; 
            
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &uint16String[1], false, true);
            break;
        
        case CONFIRM:
            header[0] = '\x00'; 
            
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, header, false, false);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &REFuint16String[0], false, true);
            lastIndexModification = index_string_concat(lastIndexModification, userInputStruct->messageToBeSend, &REFuint16String[1], false, true);
            break;

        default:
            fprintf(stderr, "ERR: while building message for server in function build_message_for_server\n");
            return -1;
        }

        userInputStruct->messageLenght = lastIndexModification;
      
    }
    
      return 0;
    
}

//  ***  FUNCTIONS EXCLUSIVE FOM TCP VARIANT    *** //

int parse_single_inputpart (char input[], int indexPosition ,int numberOfSpacesToSkip, int maxCharToRead , char *target){
    char string [SECRETLEN]; //SECRETLEN is the longest possible argument
    
    int index = 0;

    //printf("zacinam na indexu:%d\n", indexPosition);

    //increment to skip whitespaces
    indexPosition++;

        while(input[indexPosition] != '\0' )
        {

           if (input[indexPosition] == ' '|| input[indexPosition] == '\n')
           {
            string[index] = '\0';
            break;
           }
           else
           {
                string[index] = input[indexPosition];
           }

           
           index++;
           indexPosition++;

           if (index > maxCharToRead)
            {
                break;
            }
            
        }
    string[index] = '\0';

    size_t length = strlen(string);
    memcpy(target, string, length+1);   // +1 due to '\0' char
    return indexPosition;
}


int parse_single_server_message_part (char input[], int indexPosition , int maxIndexToRead , char *target, enum messageType messageType){
    char string [SECRETLEN]; //SECRETLEN is the longest possible argument
    
    int index = 0;

    //printf("zacinam na indexu:%d\n", indexPosition);

    //increment to skip whitespaces
    indexPosition++;

        while(input[indexPosition] != '\0' )
        {

           if (messageType != MSG)
           {
                if (input[indexPosition] == ' '|| input[indexPosition] == '\n')
                {
                    string[index] = '\0';
                    break;
                }
                else
                {
                        string[index] = input[indexPosition];
                }            
                index++;
                indexPosition++;
                if (indexPosition > maxIndexToRead)
                {
                    break;
                }
           }
           else if (messageType == MSG)
           {
                string[index] = input[indexPosition];           
                index++;
                indexPosition++;
                if (indexPosition > maxIndexToRead-3)
                {
                    break;
                }
           }
           
           
            
        }
    string[index] = '\0';

    size_t length = strlen(string);
    memcpy(target, string, length+1);   // +1 due to '\0' char
    return indexPosition;
}


int user_input_parse(char input[], UserInput *userInputStruct) {
    int i = 0; 
    bool commandParsed = false;

    if (input == NULL || userInputStruct == NULL) {
        fprintf(stderr, "ERR: NULL pointer in user_input_parse function\n");
        return -1;
    }

        //User insert COMMAND
        if (input[0] == '/')
        {
            
                //parsing command part
                if (commandParsed != true)
                {
                    commandParsed = true;
                    char actualCommand[8];
                    //Reads command
                    for (int j = 0; j < 8; j++)
                    {
                        char inputChar = input[j];
                        if (inputChar == ' ' || inputChar == '\n')
                        {
                            i=j;    //Move index to the end of command part
                            actualCommand[j] = '\0';
                            break;
                        }
                        else
                        {
                            actualCommand[j] = inputChar;
                        }
                    } 

                    if (strcmp(actualCommand, "/help") == 0)
                    {
                        //break;
                    }
                    else if (strcmp(actualCommand, "/auth") == 0)
                    {
                        userInputStruct->command = authCommand;
                    }
                    else if (strcmp(actualCommand, "/join") == 0)
                    {
                        userInputStruct->command = joinCommand;
                    }
                    else if (strcmp(actualCommand, "/rename") == 0)
                    {
                        userInputStruct->command = renameCommand;
                    }
                    else
                    {
                        userInputStruct->command = CommandError;
                        fprintf(stderr, "ERR: unknown command, try /help\n");
                        return 0;
                    }
                
                }

                //Specific commands have specific arguments
                switch (userInputStruct->command)
                {
                //-AUTH format {Username} {Secret} {DisplayName}
                case authCommand:
                    i = parse_single_inputpart(input, i, 1, USERNAMELEN-1, userInputStruct->username);
                    i = parse_single_inputpart(input, i, 2, SECRETLEN-1, userInputStruct->secret);
                    i = parse_single_inputpart(input, i, 3, USERNAMELEN-1, userInputStruct->displayName);
                    break;
                //-JOIN format {ChannelID}
                case joinCommand:
                    i = parse_single_inputpart(input, i, 1, USERNAMELEN-1, userInputStruct->channelID);
                    break;
                // -RENAME format {DisplayName}
                case renameCommand:
                    i = parse_single_inputpart(input, i, 1, USERNAMELEN-1, userInputStruct->displayName);
                    break;
                
                default:
                    break;
                }
                i++;
        }

        //User writes a message
        else
        {
            //memcpy(userInputStruct->message, input, strlen(input)+1);
            userInputStruct->messageType = MSG;
            userInputStruct->command = other;
        }


    return 0;
}


int server_message_parse (char *recivedString,UserInput *serverResponseStruct){
    char messageType [10];
    char buffer[MAXMESSLEN];
    buffer[MAXMESSLEN-1] = '\0';
    int RNlength = RN_strlen(recivedString);
        //printf("RNlen je: %d\n", RNlength);

    if (recivedString == NULL || serverResponseStruct == NULL)
    {
        fprintf (stderr, "ERR: Function server_message_parse recived NULL pointer\n");
        return -1;
    }

    int stringIndex = 0;

   stringIndex = parse_single_server_message_part(recivedString, -1, RNlength , messageType, OTHERmessageType);

    // printf("Messagetype is %s\n", messageType);
    // printf("Rozdilne znaky %d\n", strcmp(messageType, "REPLY"));

    to_uppercase(messageType);

    if (strcmp(messageType, "ERR") == 0)
    {
        serverResponseStruct->messageType = ERR;
    }
    else if (strcmp(messageType, "REPLY") == 0)
    {
         serverResponseStruct->messageType = REPLY;
    }
    else if (strcmp(messageType, "AUTH") == 0)
    {
         serverResponseStruct->messageType = AUTH;
    }
    else if (strcmp(messageType, "JOIN") == 0)
    {
         serverResponseStruct->messageType = JOIN;
    }
    else if (strcmp(messageType, "MSG") == 0)
    {
         serverResponseStruct->messageType = MSG;
    }
    else if (strcmp(messageType, "BYE") == 0)
    {
         serverResponseStruct->messageType = BYE;
    }
    else if (strcmp(messageType, "BYE\r\n") == 0)
    {
         serverResponseStruct->messageType = BYE;
    }
    else if (strcmp(messageType, "CONFIRM") == 0)    //Unused in TCP
    {
         serverResponseStruct->messageType = CONFIRM;
    }
    else 
    {
        fprintf(stderr, "ERR: Error in function server_message_parse while msgType assignment\n");
        // fprintf(stderr, "messageType is: %s\n", messageType );
        return -1;
    }

    switch (serverResponseStruct->messageType)
    {
    case ERR:   //ERR FROM {DisplayName} IS {MessageContent}\r\n
        //FROM
         stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
         //{DisplayName}
          stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
            strcpy(serverResponseStruct->displayName, buffer);
        //IS
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
        //{MessageContent}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, MSG);
            strcpy(serverResponseStruct->message, buffer);
        break;
    
    case REPLY: //REPLY {"OK"|"NOK"} IS {MessageContent}\r\n
        //OK or NOK
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType); 
            if (strcmp(buffer, "OK") == 0)   {serverResponseStruct->isOK = OK;}
            else                        {serverResponseStruct->isOK = NOK;}
        //IS
        stringIndex = parse_single_server_message_part(recivedString, stringIndex,RNlength ,buffer, OTHERmessageType); 
        //{Messagecontent}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex,RNlength ,buffer, MSG);
            strcpy(serverResponseStruct->message, buffer);
        break;
    
    case AUTH:  //AUTH {Username} AS {DisplayName} USING {Secret}\r\n
        //{Username}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
            strcpy(serverResponseStruct->username, buffer);
        //AS
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
        //{DisplayName}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
            strcpy(serverResponseStruct->displayName, buffer);
        //USING
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
        //{Secret}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
            strcpy(serverResponseStruct->secret, buffer);
        break;

    case JOIN:  //JOIN {ChannelID} AS {DisplayName}\r\n
        //{ChanngelID}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
            strcpy(serverResponseStruct->channelID, buffer);
        //AS
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
        //{DisplayName}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer , OTHERmessageType);
            strcpy(serverResponseStruct->displayName, buffer);
        break;

    case MSG:   //MSG FROM {DisplayName} IS {MessageContent}\r\n
        //FROM
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
        //{DisplayName}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
            strcpy(serverResponseStruct->displayName, buffer);
        //IS
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, OTHERmessageType);
        //{MessageContent}
        stringIndex = parse_single_server_message_part(recivedString, stringIndex, RNlength,buffer, MSG);
            strcpy(serverResponseStruct->message, buffer);
        break;

    case BYE:   	//BYE\r\n
        /* code */
        break;

    case CONFIRM:
        /* TODO IN UDP */
        break;

    default:
        fprintf(stderr, "ERR: Unknown messageType in function server_message_parse\n");
        return -1;
    }
    
    return 0;
}


//  ***  FUNCTIONS EXCLUSIVE FOM UDP VARIANT    *** //

void printBinary (char c){
    for (int i = 7; i >= 0; i--)
    {
        printf("%d", (c >> i) & 1);
    }
    
}

uint16_t string_to_uint16 (char *recivedString, int startIndex){
    char tmpString[3];

    tmpString[0] = recivedString [startIndex];
    tmpString[1] = recivedString [startIndex+1];
    tmpString[2] = '\0';

    uint16_t value = (tmpString[0] << 8) | tmpString[1];

    return value;
}

int parse_single_datagram_part (char *recivedString, int startIndex, char *target){
    int index = 0;

    while (recivedString[startIndex] != '\0')
    {
        target[index] = recivedString[startIndex];
        startIndex++;
        index++;
    }

    target[index] = '\0';

    return startIndex;
}

int server_datagram_parse (char *recivedString,UserInput *serverResponseStruct){

    int position = 3;   //Any message content starts at index 3

    if (recivedString == NULL || serverResponseStruct == NULL)
    {
        fprintf (stderr, "ERR: function server_message_parse recived NULL pointer\n");
        return -1;
    }


    if (recivedString[0] == '\x00')
    {
        serverResponseStruct->messageType = CONFIRM;
    }
    else if (recivedString[0] == '\x01')
    {
         serverResponseStruct->messageType = REPLY;
    }
    else if (recivedString[0] == '\x02')
    {
         serverResponseStruct->messageType = AUTH;
    }
    else if (recivedString[0] == '\x03')
    {
         serverResponseStruct->messageType = JOIN;
    }
    else if (recivedString[0] == '\x04')
    {
         serverResponseStruct->messageType = MSG;
    }
    else if (recivedString[0] == '\xFE')
    {
         serverResponseStruct->messageType = ERR;
    }
    else if (recivedString[0] == '\xFF')
    {
         serverResponseStruct->messageType = BYE;
    }
    else 
    {
        fprintf(stderr, "ERR: Error in function server_message_parse while msgType assignment\n");
        // fprintf(stderr, "messageType is: %c\n", recivedString[0] );
        return -1;
    }

    //Read MessageID
    serverResponseStruct->messageID = string_to_uint16(recivedString, 1);

    switch (serverResponseStruct->messageType)
    {
    case ERR:   
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->displayName);
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->message);
        break;
    
    case REPLY:
        serverResponseStruct->reply[0] = recivedString[3];
        serverResponseStruct->Ref_messageID = string_to_uint16(recivedString, 4);
        position = 6;
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->message);
        break;
    
    case AUTH: 
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->username);
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->displayName);
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->secret);
        break;

    case JOIN: 
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->channelID);
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->displayName);
        break;

    case MSG: 
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->displayName);
        position = parse_single_datagram_part(recivedString, position, serverResponseStruct->message);
        break;

    case BYE: 
        /* code */
        break;

    case CONFIRM:
        serverResponseStruct->Ref_messageID = serverResponseStruct->messageID;
        break;

    default:
        fprintf(stderr, "ERR: Unknown messageType in function server_message_parse\n");
        return -1;
    }
    
    return 0;
}

int store_message_id (u_int16_t arr[], int index, u_int16_t valueToBeStored){
    if (index >= 100 || index < 0)
    {
        fprintf(stderr, "ERR: Internal error message ID cant be store\n");
        return -1;
    }
    
    arr[index] = valueToBeStored;
    index++;
    if (index == 100)
    {
        index = 0;
    }

    return index;
}

bool message_was_already_recived (u_int16_t arr[], int index, u_int16_t valueToBeFound ){

    for (int i = 0; i < index; i++)
    {
        if (arr[index] == valueToBeFound)
        {
            return true;
        }
        
    }
    
    return false;
}
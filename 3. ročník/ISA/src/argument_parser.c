/**
 * File: argument_parser.c
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#include "main.h"
#include "libcap_engine.h"

void setup_argument_struct(argumentStruct *argument_struct){
    argument_struct->print_active_interfaces = false;
    argument_struct->print_help = false;
    argument_struct->domainsfile = false;
    argument_struct->domainsfile_name[0] = '\0';
    argument_struct->interface_loaded = false;
    argument_struct->interface[0] = '\0';
    argument_struct->pcap_file = false;
    argument_struct->pcap_file_name[0] = '\0';
    argument_struct->translationsfile = false;
    argument_struct->translationsfile_name[0] = '\0';
    argument_struct->verbose = false;
    argument_struct->error = false;
}

void print_help_function(){
    printf ("Uziti: ./dns-monitor (-i <interface> | -p <pcapfile>) [-v] [-d <domainsfile>] [-t <translationsfile>]\n");
    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("Spusteni bez parametru vypise dostupna rozhrani:\n");
    printf("Parametry:\n");
    printf("-i <interface> - název rozhraní, na kterém bude program naslouchat, nebo\n");
    printf("-r <pcapfile> - název souboru PCAP, který program zpracuje;\n");
    printf("-v režim verbose: kompletní výpis detailů o zprávách DNS;\n");
    printf("-d <domainsfile> - název souboru s doménovými jmény;\n");
    printf("-t <translationsfile> - název souboru s překladem doménových jmen na IP.\n");
    printf("\n");
    return;
}

void print_debug(argumentStruct *argument_struct){
    printf("---------------------------------\n");      //Debug
    printf("Loaded arguments:\n");      //Debug
    if (argument_struct->domainsfile == true)
    {
        printf("\tdomainsfile_name: %s\n", argument_struct->domainsfile_name);    //Debug
    }
    if (argument_struct->interface_loaded == true)
    {
        printf("\tinterface_name: %s\n", argument_struct->interface);      //Debug
    }
    if (argument_struct->pcap_file == true)
    {
        printf("\tpcap_file_name: %s\n", argument_struct->pcap_file_name);      //Debug
    }
    if (argument_struct->translationsfile == true)
    {
        printf("\ttranslationfile_name: %s\n", argument_struct->translationsfile_name);   //Debug
    }
    if (1)
    {
        printf("---------------------------------\n");      //Debug
    }
}


int parse_argument(int argc, char* argv[], argumentStruct *argument_struct){
    if (argv == NULL || argument_struct == NULL)
    {
        fprintf(stderr, "Error while parsing arguments, null pointer received\n");
        return -1;
    }

    int number_of_parsed_arguments = 1;
    
    setup_argument_struct(argument_struct);

    if (argc == 1)
    {
        printInterfaces();
        return 0;
    }

    //Parse all arguments
    while (number_of_parsed_arguments < argc)
    {

        //------------------------------------------------------
        //-i <interface> INTERFACE
        if (strcmp(argv[number_of_parsed_arguments], "-i") == 0)
        {
            // printf ("-i founded\n");    //Debug
            if (argument_struct->interface_loaded == true)
            {
                printf("Error while loading interface parameter - multiple arguments found\n");
                printf("Try -help for more informations\n");
                argument_struct->error = true;
                return -1;
            }
            else
            {
                argument_struct->interface_loaded = true;
                if (argv[number_of_parsed_arguments+1] != NULL)
                { 
                    strncpy(argument_struct->interface, argv[number_of_parsed_arguments+1], STRING_MAX_LEN-1);
                    number_of_parsed_arguments++;
                }
                else
                {
                    printf("Error while loading interface parameter - parameter is missing\n");
                    printf("Try -help for more informations\n");
                    argument_struct->error = true;
                    return -1;
                }
            }
            
            
            
        }

        //------------------------------------------------------
        //-p <pcapfile> PCAP file load
        else if (strcmp(argv[number_of_parsed_arguments], "-p") == 0)
        {
            // printf ("-p founded\n"); //Debug
            if (argument_struct->pcap_file == true)
            {
                printf("Error while loading PCAP parameter - multiple arguments found\n");
                printf("Try -help for more informations\n");
                argument_struct->error = true;
                return -1;
            }
            else
            {
                if (argv[number_of_parsed_arguments+1] != NULL)
                { 
                    argument_struct->pcap_file = true;
                    strncpy(argument_struct->pcap_file_name, argv[number_of_parsed_arguments+1], STRING_MAX_LEN-1);
                    number_of_parsed_arguments++;
                }
                else
                {
                    printf("Error while loading PCAP parameter - parameter is missing\n");
                    print_help_function();
                    return -1;
                }
            }
            
            
        }

        //------------------------------------------------------
        // -v   Verbose
        else if (strcmp(argv[number_of_parsed_arguments], "-v") == 0)
        {
            if (argument_struct->verbose == true)
            {
                printf("Error while loading verbose parameter - multiple arguments found\n");
                printf("Try -help for more informations\n");
                argument_struct->error = true;
                return -1;
            }
            // printf ("-v founded\n"); //Debug
            argument_struct->verbose = true;
        }

        //------------------------------------------------------
        // -d <domainsfile>
        else if (strcmp(argv[number_of_parsed_arguments], "-d") == 0)
        {
            // printf ("-d founded\n"); // Debug
            if (argument_struct->domainsfile == true)
            {
                printf("Error while loading domainsfile parameter - multiple arguments found\n");
                printf("Try -help for more informations\n");
                argument_struct->error = true;
                return -1;
            }
            else
            {
                if (argv[number_of_parsed_arguments+1] != NULL)
                {
                    argument_struct->domainsfile = true;
                    strncpy(argument_struct->domainsfile_name, argv[number_of_parsed_arguments+1], STRING_MAX_LEN-1);
                    number_of_parsed_arguments++;
                }
                else
                {
                    printf("Error while loading damainsfile parameter - parameter is missing\n");
                    print_help_function();
                    return -1;
                }
            }
            
            
        }

        //------------------------------------------------------
        // -t <translationsfile>
        else if (strcmp(argv[number_of_parsed_arguments], "-t") == 0)
        {
            // printf ("-t founded\n");    //Debug
            if (argument_struct->translationsfile == true)
            {
                printf("Error while loading translationsfile parameter - multiple arguments found\n");
                printf("Try -help for more informations\n");
                argument_struct->error = true;
                return -1;
            }
            else
            {
                if (argv[number_of_parsed_arguments+1] != NULL)
                {
                    argument_struct->translationsfile = true;
                    strncpy(argument_struct->translationsfile_name, argv[number_of_parsed_arguments+1], STRING_MAX_LEN-1);
                    number_of_parsed_arguments++;
                }
                else
                {
                    printf("Error while loading translationfile parameter - parameter is missing\n");
                    print_help_function();
                    return -1;
                }
            }
            
            
            
        }

        //------------------------------------------------------
        //-h HELP        
        else if (strcmp(argv[number_of_parsed_arguments], "-h") == 0)
        {
            // printf ("-help founded\n"); //Debug
            argument_struct->print_help = true;
            return 0;
        }

        //------------------------------------------------------
        //--help HELP
        else if (strcmp(argv[number_of_parsed_arguments], "--help") == 0)
        {
            // printf ("--help founded\n"); //Debug
            argument_struct->print_help = true;
            return 0;
        }
        else
        {
            printf("Error, given argument is not valid, unknown argument\n");
            return -1;
        }
        number_of_parsed_arguments++;
    }
    
    // print_debug(argument_struct);
    return 0;
}

/**
 * End of file argument_parser.c
 */
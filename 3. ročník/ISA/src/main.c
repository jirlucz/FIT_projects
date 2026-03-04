/**
 * File: main.c
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#include "main.h"
#include "argument_parser.h"
#include "libcap_engine.h"
#include "dns_parser.h"
#include "file_printer.h"

FILE *domainsfile_file = NULL;
FILE *translationsfile_file = NULL;

/**
 * Jiri Kotek xkotek09
 * ISA project
 * VUT FIT 2024
*/

// Signal handler function
void sigint_handler(int sig) {
    cleanup();
    exit(EXIT_SUCCESS);
}

int main (int argc, char *argv[]){
    argumentStruct struct_of_arguments;
    pcap_t *handle = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];

    //Catch Ctrl^C
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        return EXIT_FAILURE;
    }
    
    //Parse user input
    if (parse_argument(argc, argv, &struct_of_arguments) == -1)
    {
        return -1;
    }
    

        if (struct_of_arguments.pcap_file == true && struct_of_arguments.interface_loaded == true)
        {
            fprintf(stderr, "You can set only one input at a single run\n");
            return -1;
        }

        if (struct_of_arguments.print_help == true)
        {
            print_help_function();
            return 0;
        }

        if (struct_of_arguments.print_active_interfaces == true)
        {
            printInterfaces();
            return 0;
        }

        if (struct_of_arguments.domainsfile == true)
        {
            domainsfile_file = fopen(struct_of_arguments.domainsfile_name , "a+");
             if (domainsfile_file == NULL) {
                // If the file couldn't be created, print an error message
                printf("Error creating file.\n");
                return -1;
            }
        }

        if (struct_of_arguments.translationsfile == true)
        {
            translationsfile_file = fopen(struct_of_arguments.translationsfile_name , "a+");
            if (translationsfile_file == NULL) {
                // If the file couldn't be created, print an error message
                printf("Error creating file.\n");
                return -1;
            }
        }
        
        file_print_setup(domainsfile_file, translationsfile_file );
    

    if (struct_of_arguments.pcap_file == true)
    {
        handle = pcap_open_offline(struct_of_arguments.pcap_file_name, errbuf);
        setup_offline(handle, &struct_of_arguments, domainsfile_file, translationsfile_file, struct_of_arguments.verbose);
        if (handle == NULL) {
            fprintf(stderr, "Error opening pcap file %d: %s\n", struct_of_arguments.pcap_file, errbuf);
            return -1;
        }
    }
    else
    {
        //Create handle
        handle = setup_pcap_handle(handle, &struct_of_arguments, domainsfile_file, translationsfile_file, struct_of_arguments.verbose);
        
        if (handle == NULL)
        {
            fprintf(stderr, "Error while creating a handle for online capturing in main\n");
            return -1;
        }
    }
    

    
    

    
    //Start the packet capture with a set count or continually if the count is 0.
    if (pcap_loop(handle, 0, packet_handler, (u_char*)NULL) < 0) { //CHECK 0 IN ARGUMENT !!
    	fprintf(stderr, "pcap_loop failed: %s\n", pcap_geterr(handle));
	    return -1;
    }

    //Free memory
    cleanup();

    return 0;
}

/**
 * End of file main.c
 */


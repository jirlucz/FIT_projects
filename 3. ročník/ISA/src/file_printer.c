/**
 * File: file_printer.c
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#include "main.h"

FILE *global_domains_file = NULL;
FILE *global_translations_file = NULL;


void file_print_setup(FILE *domains_file, FILE *translations_file){
    if (domains_file != NULL)
    {
        global_domains_file = domains_file;
        setvbuf(global_domains_file, NULL, _IONBF, 0); // Turn off buffering
    }

    if (translations_file != NULL)
    {
        global_translations_file = translations_file;
        setvbuf(global_translations_file, NULL, _IONBF, 0); // Turn off buffering
    }
    
    
}

bool find_domains_name(u_char *domains_name){
    if (global_domains_file != NULL)
    {
        char line[MAX_DOMAIN_NAME]; 

        // Check if domains_name is valid
        char ch = '\0';
        int iterator = 0;
        ch = (u_char) domains_name[0];
        bool dot_found = false;
        while (ch != '\0')
        {
            if (ch == '.')
            {
                dot_found = true;
                break;
            }
            iterator++;
            ch = domains_name[iterator];
        }

        // If domains_name is not valid, return true, so the name wont be printed out
        if (dot_found == false)
        {
            return true;
        }
        

        rewind(global_domains_file);
        
        // Procházení každého řádku v souboru
        while (fgets(line, sizeof(line), global_domains_file)) 
        {
            // remove '\n' char 
            line[strcspn(line, "\n")] = 0;
            line[strcspn((char *) domains_name, "\n")] = 0;

            // printf("Porovnavam domains_name: |%s|\n", domains_name);
            // printf("Porovnavam line : |%s|\n",line);
            // printf("Pocet rozdilnych znaku je: %d\n", strcmp(line, (char *) domains_name));

            if (strcmp(line, (char *) domains_name) == 0)
            {
                return true;
            }
            
        }
    }

    return false;
}

bool find_translation(char *domains_name_w_ip_addr){
    if (global_translations_file != NULL)
    {
        char line[MAX_DOMAIN_NAME + MAX_IP_ADDR_LEN]; 
        rewind(global_translations_file);

        // Procházení každého řádku v souboru
        while (fgets(line, sizeof(line), global_translations_file)) 
        {
            // remove '\n' char 
            line[strcspn(line, "\n")] = 0;
            line[strcspn(domains_name_w_ip_addr, "\n")] = 0;

            // printf("Porovnavam domains_name: |%s|\n", domains_name_w_ip_addr);
            // printf("Porovnavam line : |%s|\n",line);
            // printf("Pocet rozdilnych znaku je: %d\n", strcmp(line, domains_name_w_ip_addr));

            if (strcmp(line, domains_name_w_ip_addr) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

void write_to_file(u_char *domains_name, char *ip_address){
    if (global_domains_file != NULL && domains_name != NULL)
    {
        if (find_domains_name(domains_name) == false)
        {
            fprintf(global_domains_file, "%s", domains_name);
            fprintf(global_domains_file, "\n");
            fflush(global_domains_file);
        }
        
    }

    if (global_translations_file != NULL && domains_name != NULL && ip_address != NULL)
    {
        char new_line[MAX_DOMAIN_NAME + MAX_IP_ADDR_LEN];

        new_line[0] = '\0';
        strcpy((char *) new_line,(char *) domains_name);
        strcat((char *) new_line," ");
        // printf("NEWLINE PRINTER: |%s|\n", new_line);
        // printf("IPADDR PRINTER: |%s|\n", ip_address);
        strcat((char *) new_line,(char *) ip_address);

        if (find_translation(new_line) == false)
        {
            fprintf(global_translations_file, "%s",new_line);
            fprintf(global_translations_file, "\n");
            fflush(global_translations_file);
        }
        
    }

}

/**
 * End of file file_printer.c
 */
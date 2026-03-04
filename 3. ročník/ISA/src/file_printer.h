/**
 * File: file_printer.h
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#ifndef FILE_PRINTER_H
#define FILE_PRINTER_H

#define MAX_IP_ADDR_LEN 50

/**
 * @brief Function sets global pointers to the files
 * 
 * @param domains_file Sets file with domain names
 * @param translations_file Sets file with translations 
 */
void file_print_setup(FILE *domains_file, FILE *translations_file);


/**
 * @brief Function check if a domain name is already in textfile
 * 
 * @param domains_name a string which function tries to find in a file that contain domain names
 * 
 * @returns true if domain name is already in a file, otherwise false
 */
bool file_domains_name(u_char *domains_name);


/**
 * @brief Function check if a domain name with a IP address is already in textfile
 * 
 * @param domains_name_w_ip_addr a string in format <domain_name><space><IP_address> 
 *  
 * @returns true if a record is already in a file, otherwise false
 */
bool find_translation(char *domains_name_w_ip_addr);

/**
 * @brief Function tries to write domain name with ip address to setted files
 * 
 * @param domain_name domain name
 * @param ip_address IPv4/IPv6 address to write into a file, if NULL writes only domain_name into domain_file (without IP_address) 
 */
void write_to_file(u_char *domains_name, char *ip_address);

#endif /* FILE_PRINTER_H */
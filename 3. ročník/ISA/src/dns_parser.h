/**
 * File: dns_parser.h
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#ifndef DNS_PARSER_H
#define DNS_PARSER_H

#include "file_printer.h"

/**
 * @brief Function deallocates all reserved memory in dns_parser
 */
void dns_parser_clean();

void malloc_NAME_space(struct_packet_info* packet_info);


/**
 * @brief Function sets up all needed data structures and allocates memory
 * 
 * @param packet_raw pointer to <u_char> where is stored captured packet
 * @param verbose <bool>, given parameter fro the user
 */
void dns_parser_setup(struct_packet_info* packet_info, u_char *packet_raw, bool verbose);

/**
 * @brief Function parses question part of dns packet
 * 
 * @param packet_info pointer to <struct_packet_info> 
 * @param packet_raw pointer to the captured packet
 */
void parse_question_section(struct_packet_info* packet_info, u_char *packet_raw);


/**
 * @brief Help function which calls parse_question_section() and parse_resource_record_section() function in right order
 
 * @param packet_info pointer to <struct_packet_info> 
 * @param packet_raw pointer to the captured packet
 */
void fill_non_header_dns_parts(struct_packet_info* packet_info, u_char *packet_raw, bool verbose);


/**
 * @brief Function parses RR parts of dns packet
 * 
 * @param packet_info pointer to <struct_packet_info> 
 * @param packet_raw pointer to the captured packet
 * @param NoRecords Number of section of selected RR part
 * @param destination pointer to <resource_record_section> where will be stored output, must be allocated
 * @param type_of_record_enum <enum> select  dns part to be parsed Answer = 0, Authority,Additional
 * 
 * @returns pointer to the following RR part 
 */
u_char* parse_resource_record_section (struct_packet_info* packet_info, u_char *packet_raw, u_int16_t NoRecords, resource_record_section *destination, RR_type type_of_record_enum );

/**
 * @brief function load a domain name in source and stores it into destination
 * 
 * @param destination pointer to <u_char> where will be stored the result of conversion, destination must be allocated
 * @param source pointer to <u_char> where starts a domain name in a captured packet
 * @param name_length optional, length of the domain name, read maximum of name_length characters, if set to 0 read whole domain name
 * @param packet_raw pointer to the captured packet
 * @param packet_info pointer to <struct_packet_info> 
 * @param save_to_file if true domain name will be written into a file, used for not printing RNAME in SOA record
 * 
 * @returns length of domain name part in the packet
 */
uint16_t load_domain_name (u_char *destination,const u_char *source, uint16_t name_length, u_char *packet_raw, struct_packet_info *packet_info, bool save_to_file);

/**
 * @brief Function converts 4-byte <u_char> string into 32-bit integer
 * 
 * @param arr pointer to <u_char> with 4 bytes of data to convert
 * 
 * @returns 32bit integer
 * 
 */
int32_t u_CharArr_4_Int32(u_char *arr);

/**
 * @brief Function converts 4 byte <unsigned char> string into <char> printable string
 * 
 * @param ipv6_bytes input, pointer to <const unsigned char>
 * @param ipv6_str output, pointer to the allocated string of <char>
 */
void convert_ipv6_to_str(const unsigned char *ipv6_bytes, char *ipv6_str);

/**
 * @brief Function converts 2 byte <unsigned char> string into <char> printable string
 * 
 * @param arr pointer to <u_char> with 2 bytes of data to convert
 * 
 * @returns 16 bit integer
 * 
 */
uint16_t u_CharArr_2_u_Int16(const unsigned char *byteArray);


/**
 * @brief Function prints on STDOUT ASCII dns class given in hexadecimal argument
 * 
 * @param class_arr Array of u_char of size 3, stores hexadecimal value of dns class
 */
void print_dns_class (u_char class_arr[3]);


/**
 * @brief Function prints on STDOUT ASCII dns type given in hexadecimal representation argument
 * 
 * @param class_arr Array of u_char of size 3, stores hexadecimal value of dns type
 */
void print_dns_type(u_char type_arr[3]); 


/**
 * @brief Function prints on STDOUT detailed informations about captured packet
 *  
 * @param packet_info pointer to <struct_packet_info> 
 * @param verbose <bool>, if verbose == true, function prints detailed info otherwise does nothing  
 */
void print_verbose_dns_data (struct_packet_info* packet_info, bool verbose);

/**
 * @brief Function parses MX part of dns
 * 
 * @param packet_info pointer to <struct_packet_info> 
 * @param destination pointer to allocated MX_record_struct 
 * @param packet_raw pointer to the captured packet
 */
void parse_MX(struct_packet_info* packet_info, MX_record_struct* destination, u_char* position, u_char *packet_raw);

/**
 * @brief Function parses SRV part of dns
 * 
 * @param packet_info pointer to <struct_packet_info> 
 * @param destination pointer to allocated SRV_record_struct 
 * @param packet_raw pointer to the captured packet
 */
void parse_SRV(struct_packet_info* packet_info, MX_record_struct* destination, u_char* position, u_char *packet_raw);

/**
 * @brief Function parses SOA part of dns
 * 
 * @param packet_info pointer to <struct_packet_info> 
 * @param destination pointer to allocated SOA_record_struct 
 * @param packet_raw pointer to the captured packet
 */
void parse_SOA(struct_packet_info* packet_info, MX_record_struct* destination, u_char* position, u_char *packet_raw);


/**
 * @brief Function starts at input of <u_char> pointer and tries to find nearest '0x00' char
 * @param string pointer to the first index of domain name
 * @param packet_info pointer to the structure where are stored all information of the received packet
 * @param packet_raw pointer to the raw captured packet
 * @param set_bool pointer to <bool>, stores "true" if domain_name is given by pointer, otherwise sets "false"
 * @param set_offset stored offset, where start pointed domain name
 * 
 * @returns length of the domain name part in captured packet
 */
uint16_t name_length(const u_char *string, struct_packet_info *packet_info, u_char *packet_raw, bool *set_bool_name_ptr, u_int16_t *set_offset);


#endif /* DNS_PARSER_H */
/**
 * File: libcap_engine.h
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */


#ifndef LIBCAP_ENGINE_H
#define LIBCAP_ENGINE_H


/**
 * @brief Function prints available interfaces
 * Used when program is ran without params
*/ 
void printInterfaces() ;


/**
 * @brief Function creates a string format of filter used lated in function setup_pcap_handle() -> pcap_compile()
 * 
 * @param string    Pointer to a string (must be allocated) where will be filter stored
 * @param struct_of_arguments   Pointer to an argumentStruct that contains user-provided arguments  
*/
void create_filter(char *string, argumentStruct *struct_of_arguments);

int setup_offline(pcap_t *handle, argumentStruct *struct_of_arguments, FILE *domainsfile_file_ptr, FILE *translationsfile_file_ptr, bool verbose);

/**
 * @brief Function creates a packet capture handle
 * 
 * @param   handle  Pointer to pcap_t
 * @param   struct_of_arguments   Pointer to an argumentStruct that contains user-provided arguments 
 * 
 * @returns Pointer to a pcap_t, if error occur returns NULL
*/
pcap_t* setup_pcap_handle(pcap_t *handle, argumentStruct *struct_of_arguments, FILE *domainsfile_file_ptr, FILE *translationsfile_file_ptr, bool verbose);


/**
 * @brief Function gets the link header type and size
 * 
 * @param handle Pointer to initialized pcap_t
*/
void get_link_header_len(pcap_t* handle);


/**
 * @brief Call back function, handles incoming network packets and prints details
 * 
 * @param user Pointer to user-defined data passed to the packet_handler function
 * @param packet_header Pointer to the pcap packet header structure
 * @param packet_ptr Pointer to the packet data
*/
void packet_handler(u_char *user, const struct pcap_pkthdr *packet_header, const u_char *packet_ptr);


/**
 *  @brief Function clears all dynamically allocated memory
 *  Pointers are global variables
*/
void cleanup();


/**
 * @brief Function prints out actual timestamp with time zone
*/
void print_timestamp();


/**
 * @brief prints out source and destination MAC address of the packet
 *  
 * @param eth_header Pointer to ether_header struct of the packet
*/
void print_MAC_addresses(struct ether_header *eth_header) ;


/**
 * @brief Function prints out received packet length in bytes
 * 
 * @param packet_header Pointer to a pcap_pkthdr struct of the received packet
*/
void print_frame_length(const struct pcap_pkthdr *packet_header);


/**
 * @brief Function prints out source and destination IP addresses of received packet
 * 
 * Function uses  global variable "packet_raw" for detecting ARP packets
 * 
 * @param ip_header     Pointer to a ip struct of received packet
*/
void print_ip_address(struct ip* ip_header); 


/**
 * @brief Function prints out source and destination port number
 * 
 * @param ip_header Pointer to ip struct, used for detecting packet protocol
 * @param packet_ptr Pointer to u_char, pointer to the beginning of the packet data
*/
void print_port_number(struct ip* ip_header, const u_char *packet_ptr);


/**
 * @brief Function prints out data received in the packet
 * 
 * Function uses global variable "packet_raw" for printing received data
 * In the left part are bytes printed in hexadecimal value
 * In the right part are byes printed in ASCII value, or as "." of are non-printable
 * 
 * @param packet_header Pointer to a pcap_pkthdr struct, function uses this argument for getting caplen
*/
void print_packet_data(const struct pcap_pkthdr *packet_header);


/**
 * @brief TODO
*/
void fill_QAAA(struct_packet_info *packet_info);

#endif /* LIBCAP_ENGINE_H */

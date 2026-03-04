/**
 * File: main.h
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#ifndef MAIN_H
#define MAIN_H


#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pcap.h>
#include <ctype.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pcap/pcap.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <signal.h>
#include <netinet/ip6.h>
#include <stdint.h>

#include "file_printer.h"

#define MAX_DOMAIN_NAME 1024
#define STRING_MAX_LEN 512
#define MAX_FILTER_LENGTH 1024
#define SNAPLEN_SIZE 262144 //Value from manual
#define TO_MS_TIMEOUT 50 //ms can be set btw 1-10-100 CHECK!

//Struct for storing user-provided input
typedef struct 
{
    bool print_active_interfaces;
    bool interface_loaded;
        char interface [STRING_MAX_LEN];
    bool pcap_file;
        char pcap_file_name [STRING_MAX_LEN];
    bool verbose;
    bool domainsfile;
        char domainsfile_name [STRING_MAX_LEN];
    bool translationsfile;
        char translationsfile_name [STRING_MAX_LEN];
    bool print_help;
    bool error;

} argumentStruct;

typedef struct {
    u_int32_t total_packet_length;
    u_int32_t link_header;
    u_int32_t ip_length;
    uint16_t udp_length;
    u_char *dns_header_ptr;
} packet_headers_length;

typedef struct {
    u_char Identifier [3];
    char QR_record [2];
    char OP_code [5];
    char AA [2];
    char TC [2];
    char RD [2];
    char RA [2];
    char Z [2];
    char CD [2];
    char AD [2];
    char AC [2];
    char RCODE [5];
    u_int16_t NoQuestions;
    u_int16_t NoAnswers;
    u_int16_t NoAuthoritys;
    u_int16_t NoAdditions;


} dns_record;

typedef struct {
    u_char      *QNAME; //malloc
    u_char      QTYPE[3];
    u_char      QCLASS[3];
    u_char      *end_question_ptr;
} question_section;

typedef enum {
    Answer = 0,
    Authority,
    Additional
}RR_type;

typedef struct {
    u_char      M_NAME[MAX_DOMAIN_NAME]; 
    u_char      R_NAME[MAX_DOMAIN_NAME]; 
    u_int32_t   SERIAL;
    u_int32_t     REFRESH;
    u_int32_t     RETRY;
    u_int32_t     EXPIRE;
    u_int32_t     MINIMUM;
    u_int32_t     MAX_TTL;
} SOA_record_struct;

typedef struct {
    u_char      EXCHANGE[MAX_DOMAIN_NAME]; 
    uint16_t   PREFERENCE;
} MX_record_struct;

typedef struct {
    u_char     TARGET[MAX_DOMAIN_NAME];
    u_char     NAME[MAX_DOMAIN_NAME];   
    uint16_t   PRIORITY;
    uint16_t   WEIGHT;
    uint16_t   PORT;
} SRV_record_struct;

typedef struct {
    u_char      NAME[MAX_DOMAIN_NAME]; 
    bool        name_given_by_pointer;
    u_char      TYPE[3];
    u_char      CLASS[3];
    int32_t     TTL;
    u_int16_t   RDLENGTH;
    u_char      *RDATA; //malloc
    u_char      *end_record_ptr;
    RR_type     type_of_record;
    SOA_record_struct SOA_record;
    MX_record_struct MX_record;
    SRV_record_struct SRV_record;
} resource_record_section;

typedef struct {
    question_section    *arr_questions;             //malloc
    resource_record_section      *arr_answers;      //malloc
    resource_record_section   *arr_authorities;     //malloc
    resource_record_section  *arr_additional;       //malloc
} non_header;

typedef struct {
    packet_headers_length packet_lengths;
    u_int16_t src_port;
    u_int16_t dst_port;
    char src_ip [50];
    char dst_ip [50];
    char time_info [100];
    char proto_type [4];
    dns_record dns_record_struct;
    non_header dns_non_header_struct;
} struct_packet_info;

// Signal handler function
void sigint_handler(int sig);


#endif /* MAIN_H */

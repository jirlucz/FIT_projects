/**
 * File: libcap_engine.c
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#include "main.h"
#include "dns_parser.h"

//Used for MLD packets
#define MY_IPPROTO_SSCOPMCE 128
#define link_header_length 14   //14 for Ethernet link header length

//Global variables
FILE *global_domainsfile_ptr = NULL;
FILE *global_translationsfile_ptr = NULL;
int ip_header_length;
u_char *packet_raw  = NULL;
pcap_t *global_handle  = NULL;
pcap_if_t *global_interfaces  = NULL;
argumentStruct *global_argument_struct_ptr = NULL;
bool global_verbose_set = false;
int GLOBAL_number_of_packets = 0;   // Debug

void init_packet_info(struct_packet_info *packet_info){
    // Initialize scalar members
    packet_info->src_port = 0;
    packet_info->dst_port = 0;
    memset(packet_info->src_ip, 0, sizeof(packet_info->src_ip));
    memset(packet_info->dst_ip, 0, sizeof(packet_info->dst_ip));
    memset(packet_info->time_info, 0, sizeof(packet_info->time_info));
    memset(packet_info->proto_type, 0, sizeof(packet_info->proto_type));

    // Initialize packet_lengths
    packet_info->packet_lengths.total_packet_length = 0;
    packet_info->packet_lengths.link_header = 0;
    packet_info->packet_lengths.ip_length = 0;
    packet_info->packet_lengths.udp_length = 0;
    packet_info->packet_lengths.dns_header_ptr = NULL; // No allocation yet

    // Initialize dns_record_struct
    memset(packet_info->dns_record_struct.Identifier, 0, sizeof(packet_info->dns_record_struct.Identifier));
    memset(packet_info->dns_record_struct.QR_record, 0, sizeof(packet_info->dns_record_struct.QR_record));
    // Initialize other members...

    // Initialize non_header struct
    packet_info->dns_non_header_struct.arr_questions = NULL; // To be allocated later
    packet_info->dns_non_header_struct.arr_answers = NULL; // To be allocated later
    packet_info->dns_non_header_struct.arr_authorities = NULL; // To be allocated later
    packet_info->dns_non_header_struct.arr_additional = NULL; // To be allocated later

    return;
}

void cleanup (){
    if (packet_raw != NULL)
    {
        free (packet_raw);
        packet_raw = NULL;
    }
    if (global_domainsfile_ptr != NULL)
    {
        fclose(global_domainsfile_ptr);
        global_domainsfile_ptr = NULL;
    }
    if (global_translationsfile_ptr != NULL)
    {
        fclose(global_translationsfile_ptr);
        global_translationsfile_ptr = NULL;
    }

    pcap_close(global_handle);
    dns_parser_clean();
    
}



void printInterfaces() {
    pcap_if_t *interfaces;
    char error_buffer[PCAP_ERRBUF_SIZE];

    //Finds all available interfaces
    if (pcap_findalldevs(&interfaces, error_buffer) == -1) 
    {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", error_buffer);
        return;
    }

    printf("Available Interfaces:\n");

    //Iterate through the list of interfaces
    for (pcap_if_t *iface = interfaces; iface != NULL; iface = iface->next) {
        printf("\tName: %s\n", iface->name);
        if (iface->description)
        {
            printf("\tDescription: %s\n", iface->description);
        }
        else
        {
            printf("\tDescription: No description available\n");
        }
        printf("\n");
    }

    //Free list of interfaces
    pcap_freealldevs(interfaces);
}



void create_filter(char *string, argumentStruct *struct_of_arguments){
    //Check for NULL ptr
    if (string == NULL || struct_of_arguments == NULL)
    {
        fprintf (stderr, "Error function create_filter received NULL pointer\n ");
        return;
    }

    //Make sure string ends at index 0
    string[0] = '\0';
  
    // Construct the filter for UDP packets on port 53
    sprintf(string, "udp and port 53");


    // printf ("Debug: Filter is: %s\n", string); //Debug
    
}



void check_interface (pcap_if_t* interfaces, char *given_interface){
    bool interface_found_in_list = false;
    while (interfaces->next != NULL)
    {
        
        if (strcmp(interfaces->name, given_interface) == 0)
        {
            interface_found_in_list = true;
            break;
        }
        
        interfaces = interfaces->next;
    }

    if (interface_found_in_list != true)
    {
        fprintf (stderr, "Interface not found \n");
    }
}

int setup_offline(pcap_t *handle, argumentStruct *struct_of_arguments, FILE *domainsfile_file_ptr, FILE *translationsfile_file_ptr, bool verbose){
    global_domainsfile_ptr = domainsfile_file_ptr;
    global_translationsfile_ptr = translationsfile_file_ptr;
    global_verbose_set = verbose;

    // Apply the filter for UDP port 53 (DNS)
    global_argument_struct_ptr = struct_of_arguments;

    struct bpf_program fp;  // BPF filter program
    const char *filter_exp = "udp port 53"; // Filter string for UDP port 53
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Error compiling filter: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return -1;
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Error setting filter: %s\n", pcap_geterr(handle));
        pcap_freecode(&fp);
        pcap_close(handle);
        return -1;
    }

    // Free filter program after applying it
    pcap_freecode(&fp);
    global_handle = handle;
    return 0;
}

pcap_t* setup_pcap_handle(pcap_t *handle, argumentStruct *struct_of_arguments, FILE *domainsfile_file_ptr, FILE *translationsfile_file_ptr, bool verbose){
    global_domainsfile_ptr = domainsfile_file_ptr;
    global_translationsfile_ptr = translationsfile_file_ptr;
    global_verbose_set = verbose;

    char filter_string [MAX_FILTER_LENGTH];
    char error_buffer [PCAP_ERRBUF_SIZE];
    struct bpf_program bpf;         //Dynamical allocates
    
    pcap_if_t* interfaces = NULL;   //Dynamical allocates
    bpf_u_int32 netmask;
    bpf_u_int32 source_IP_addr;
    global_argument_struct_ptr = struct_of_arguments;

    create_filter(filter_string, struct_of_arguments);

    //Get all available interfaces
    if (pcap_findalldevs(&interfaces, error_buffer)) {
            fprintf(stderr, "pcap_findalldevs(): %s\n", error_buffer);
            pcap_freealldevs(interfaces);
            return NULL;
    }

    check_interface(interfaces, struct_of_arguments->interface);



    //Get network device source IP address and netmask
    if (pcap_lookupnet(struct_of_arguments->interface, &source_IP_addr, &netmask, error_buffer) == PCAP_ERROR) {
        fprintf(stderr, "pcap_lookupnet: %s\n", error_buffer);
        pcap_freealldevs(interfaces);
        return NULL;
    }


    // Open the device for live capture.
    handle = pcap_open_live(struct_of_arguments->interface, SNAPLEN_SIZE, 1, 1000, error_buffer);
    if (handle == NULL) {
        fprintf(stderr, "pcap_open_live(): %s\n", error_buffer);
        pcap_freealldevs(interfaces);
        return NULL;
    }

    //Convert the packet filter expression into a packet filter binary.
    if (pcap_compile(handle, &bpf, filter_string, 0, netmask) == PCAP_ERROR) {
        fprintf(stderr, "pcap_compile(): %s\n", pcap_geterr(handle));
        pcap_freealldevs(interfaces);
        return NULL;
    }
    
    // Bind the packet filter to the libpcap handle.
    if (pcap_setfilter(handle, &bpf) == PCAP_ERROR) {
        fprintf(stderr, "pcap_setfilter(): %s\n", pcap_geterr(handle));
        pcap_freealldevs(interfaces);
        return NULL;
    }
    
    pcap_freecode(&bpf);
    pcap_freealldevs(interfaces);
    global_handle = handle;
    return handle;

}




void print_timestamp(struct_packet_info* packet_info){
    char time_str[60];

    //Get the current time
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = gmtime(&raw_time);

    //Get the time zone
    // struct tm *timezone_info;
    // timezone_info = localtime(&raw_time);

    // Get the time zone offset in hours and minutes
    // int offset_hours = timezone_info->tm_gmtoff / 3600;
    // int offset_minutes = (timezone_info->tm_gmtoff % 3600) / 60;
    // char timezone_str[10];

    // Print the time zone in the format "+HH:MM" or "-HH:MM"
    // sprintf(timezone_str, "%c%02d:%02d", (offset_hours >= 0) ? '+' : '-', abs(offset_hours), abs(offset_minutes));

    //Format the time
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

    if (global_argument_struct_ptr == NULL)
    {
        fprintf (stderr, "Function print_timestamp received NULL prt to global_agrument_struct\n");
        return;
    }
    
    strcpy (packet_info->time_info, time_str);
    return;

    if (global_argument_struct_ptr->verbose == false)
    {
        //  printf ("%s%s ", time_str,timezone_str);               //Debug
         strcpy (packet_info->time_info, time_str);

    }
    else
    {
        char new_line = '\n';
        //  printf ("Timestamp: %s%s\n", time_str,timezone_str); //Debug
         packet_info->time_info[0] = '\0';
         strcpy(packet_info->time_info, "Timestamp: ");
         strcpy(packet_info->time_info, time_str);
         strcpy(packet_info->time_info, &new_line);
    }
    
    

   // printf ("%s%s\n", time_str,timezone_str);               //in RFC 3339 format 
}




void print_MAC_addresses(struct ether_header *eth_header) {

    // Skip the datalink layer header and get the EtherType.
    uint16_t ethertype = ntohs(((struct ether_header*)packet_raw)->ether_type);

    unsigned char source_mac[ETHER_ADDR_LEN];
    unsigned char destination_mac[ETHER_ADDR_LEN];
    
    // printf ("ethertype is %d\n", ethertype);
    //Check for IPv6 
    if (ethertype == ETHERTYPE_IPV6)
    {
        struct ether_header *eth_header = (struct ether_header *)packet_raw;

        // Printing source MAC address
        printf("src MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            eth_header->ether_shost[0], eth_header->ether_shost[1],
            eth_header->ether_shost[2], eth_header->ether_shost[3],
            eth_header->ether_shost[4], eth_header->ether_shost[5]);

        // Printing destination MAC address
        printf("dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            eth_header->ether_dhost[0], eth_header->ether_dhost[1],
            eth_header->ether_dhost[2], eth_header->ether_dhost[3],
            eth_header->ether_dhost[4], eth_header->ether_dhost[5]);

    return;
}


    // IPv4
    
    if (ethertype == ETHERTYPE_ARP)
    {
        struct ether_arp *arp_header;
        // If this is an ARP packet
        // Extract ARP header
        arp_header = (struct ether_arp *)(packet_raw + ETHER_HDR_LEN);
        
        // Extract source MAC address from ARP header
       
        memcpy(source_mac, arp_header->arp_sha, ETHER_ADDR_LEN);
        memcpy(destination_mac, arp_header->arp_tha, ETHER_ADDR_LEN);
        
        // Print the MAC addresses
        printf("src MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               source_mac[0], source_mac[1], source_mac[2],
               source_mac[3], source_mac[4], source_mac[5]);
        printf("dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               destination_mac[0], destination_mac[1], destination_mac[2],
               destination_mac[3], destination_mac[4], destination_mac[5]);

       
        return;
    }
    
    struct ether_header *eth_header2 = (struct ether_header *)packet_raw;
    // Check if the packet is an IPv4 packet
    if (ntohs(eth_header2->ether_type) == ETHERTYPE_IP) {
        // Assuming Ethernet header size is 14 bytes
        unsigned char *src_mac = eth_header2->ether_shost;
        unsigned char *dst_mac = eth_header2->ether_dhost;

        printf("src MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
        printf("dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
        return;
    }

    
    // // Extract source and destination MAC addresses
    // unsigned char *src_mac = eth_header->ether_shost;
    // unsigned char *dst_mac = eth_header->ether_dhost;

    // // Print source MAC address
    // printf("src MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //         src_mac[0], src_mac[1], src_mac[2],
    //         src_mac[3], src_mac[4], src_mac[5]);

    // // Print destination MAC address
    // printf("dst MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //         dst_mac[0], dst_mac[1], dst_mac[2],
    //         dst_mac[3], dst_mac[4], dst_mac[5]);
}


void fill_frame_length(const struct pcap_pkthdr *packet_header, struct_packet_info *packet_info){
    packet_info->packet_lengths.total_packet_length = packet_header->caplen;
    // printf ("frame length: %d bytes\n", packet_header->caplen);            //in bytes //debug
}


void print_ip_address(struct ip* ip_header, struct_packet_info *packet_info){
    char source_ip[INET6_ADDRSTRLEN];
    char destination_ip[INET6_ADDRSTRLEN];
    // uint16_t ethertype = ntohs(((struct ether_header*)packet_raw)->ether_type);

    if (ip_header->ip_v == 4) 
    {
        // IPv4
        strcpy(source_ip, inet_ntoa(ip_header->ip_src));
        strcpy(destination_ip, inet_ntoa(ip_header->ip_dst));
        packet_info->packet_lengths.ip_length = (ip_header->ip_hl)*4;
    } 

    else if (ip_header->ip_v == 6) 
    {
        // IPv6
        struct ip6_hdr *ip6_header;
        ip6_header = (struct ip6_hdr*)(packet_raw + link_header_length);
        inet_ntop(AF_INET6, &(ip6_header->ip6_src), source_ip, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip6_header->ip6_dst), destination_ip, INET6_ADDRSTRLEN);
        packet_info->packet_lengths.ip_length  = 40; //check
    } 

    else 
    {
        // Not IPv4 or IPv6
        //Should not occur
        // printf("ip_header is %d\n",ip_header->ip_v );
        strcpy(source_ip, "Unknown");
        strcpy(destination_ip, "Unknown");
    }


    if (global_argument_struct_ptr == NULL)
    {
        fprintf(stderr, "Function print_ip_address received null ptr to global_struct_argument \n");
    }

    packet_info->src_ip[0] = '\0';
    packet_info->dst_ip[0] = '\0';

    if (global_argument_struct_ptr->verbose == false)
    {
        // printf("%s", source_ip);
        // printf("%s ", destination_ip);
        strcpy (packet_info->src_ip, source_ip);
        strcpy (packet_info->dst_ip, destination_ip);
    }
    else
    {
        // printf ("SrcIP: %s\n", source_ip);                  //if any + IPv6
        // printf ("DstIP: %s\n", destination_ip);             //if any + IPv6
        char string_srcIp [] = "SrcIP: ";
        char string_desIp [] = "DesIP: ";
        strcpy (packet_info->src_ip, string_srcIp);
        strcpy (packet_info->dst_ip, string_desIp);
        strcpy (packet_info->src_ip, source_ip);
        strcpy (packet_info->dst_ip, destination_ip);
    }
    
    
}


void print_port_number(struct ip* ip_header, const u_char *packet_ptr, struct_packet_info* packet_info){
    struct tcphdr* tcp_header;
    struct udphdr* udp_header;
    
    if (ip_header->ip_p == IPPROTO_TCP) 
    {
        tcp_header = (struct tcphdr*)(packet_ptr + ip_header->ip_hl * 4);   //gives the length of the IP header in bytes
        // printf ("src port: %d\n", ntohs(tcp_header->th_sport));
        // printf ("dst port: %d\n", ntohs(tcp_header->th_dport));
        packet_info->src_port = ntohs(tcp_header->th_sport);
        packet_info->dst_port = ntohs(tcp_header->th_dport);
        char proto_type [] = "TCP/";
        strcpy (packet_info->proto_type, proto_type);
        
    }

    else if (ip_header->ip_p == IPPROTO_UDP) 
    {
        udp_header = (struct udphdr*)(packet_ptr + ip_header->ip_hl * 4); //gives the length of the IP header in bytes

        // printf("Zacatek UDP hlavicky je: %02x %02x %02x %02x\n", *(unsigned char *) ((packet_ptr + ip_header->ip_hl * 4) + 0), *(unsigned char *) ((packet_ptr + ip_header->ip_hl * 4) + 1), *(unsigned char *) ((packet_ptr + ip_header->ip_hl * 4) + 2), *(unsigned char *) ((packet_ptr + ip_header->ip_hl * 4) + 3)); //Debug
        // printf ("src port: %d\n", ntohs(udp_header->uh_sport));
        // printf ("dst port: %d\n", ntohs(udp_header->uh_dport));
        packet_info->src_port = ntohs(udp_header->uh_sport);
        packet_info->dst_port = ntohs(udp_header->uh_dport);
        char proto_type [] = "UDP/";
        strcpy (packet_info->proto_type, proto_type);

        uint16_t ip_total_length = ntohs(ip_header->ip_len); // Total length in the IP header
        int udp_header_length = sizeof(struct udphdr);
        uint16_t udp_payload_length = ip_total_length - (int)udp_header_length;

        // packet_info->packet_lengths.udp_length = ntohs(udp_header->len);
        packet_info->packet_lengths.udp_length = (udp_payload_length-12);

        u_int32_t hlavicka = 8;
        packet_info->packet_lengths.udp_length = (packet_info->packet_lengths.udp_length) - hlavicka;

        // u_int32_t integer = ntohs(udp_header->len) -8 ;
        // printf ("Delka uh_ulen UDP: %d\n", ntohs(udp_header->uh_ulen));

    }

    else 
    {
        fprintf(stderr, "Unknown protokol, cant find port number\n");
    }
}


void print_packet_data(const struct pcap_pkthdr *packet_header){
    int byte_offset;
    int byte_offset_2;
    int byte_offset_2_backup = 0;
    //Print hexadecimal format of received data 
    for (byte_offset = 0; byte_offset < packet_header->caplen; byte_offset++) {
        // printf ("Offset je %d\n", byte_offset);
        if (byte_offset % 16 == 0) {
            printf("0x%04X: ", byte_offset);
        }
        printf("%02X ", packet_raw[byte_offset]);
        if ((byte_offset + 1) % 16 == 0) {
            printf ("\t");
            for (byte_offset_2 = byte_offset_2_backup; byte_offset_2 < packet_header->caplen; byte_offset_2++) {
                if (isprint(packet_raw[byte_offset_2])) {
                    printf("%c ", packet_raw[byte_offset_2]);
                } else {
                    printf(". ");
                }
                if ((byte_offset_2 + 1) % 16 == 0) {
                    byte_offset_2_backup = byte_offset_2+1;
                    printf("\n");
                    // printf("END3\n");
                    break;
                }
            }
            // printf("END\n");
        }
    }

    //Part for printing ASCII format of received data
    if (byte_offset % 16 != 0) {
        for (int i = 0; i < (16-(byte_offset % 16)); i++)
        {
            printf ("   ");
        }
        printf ("\t");
        for (byte_offset_2 = byte_offset_2_backup; byte_offset_2 < packet_header->caplen; ++byte_offset_2) {
            //Check if char printable
            if (isprint(packet_raw[byte_offset_2])) {
                printf("%c ", packet_raw[byte_offset_2]);
            } else {
                printf(". ");
            }
            if ((byte_offset_2 + 1) % 16 == 0) {
                byte_offset_2_backup = byte_offset_2+1;
                
                // printf("END4\n");
                break;
            }
        }
        printf("\n");
        // printf("END2\n");
    }
}

void print_short_bits (const u_char *packet_ptr, const struct pcap_pkthdr *packet_header ){
    if (packet_header == NULL || packet_ptr == NULL)
    {
        fprintf (stderr, "Error, function print_short_bits received NULL ptr\n");
        return;
    }


    
    
}

void fill_dns_flags (struct_packet_info *packet_info){
    u_char dns_flags [3];
        dns_flags[0] = *(unsigned char *)((packet_info->packet_lengths.dns_header_ptr)+2);
        dns_flags[1] = *(unsigned char *)(packet_info->packet_lengths.dns_header_ptr+3);
        dns_flags[2] = '\0';
    // printf("Hexadecimal values2: %02x %02x\n",dns_flags[0], dns_flags[1] );

    //QR message
    if (((dns_flags[0] >> 7) & 1) == 0)
    {
        packet_info->dns_record_struct.QR_record[0] = 'Q';
        packet_info->dns_record_struct.QR_record[1] = '\0';
    }
    else
    {
        packet_info->dns_record_struct.QR_record[0] = 'R';
        packet_info->dns_record_struct.QR_record[1] = '\0';
    }

    //OP code
        // Loop over each bit in the byte, starting from the most significant bit (7th) down to 0
            // for (int i = 7; i >= 0; --i) {
            //     // Use bitwise AND and shift to isolate each bit
            //     putchar((dns_flags[0] & (1 << i)) ? '1' : '0');
            // }
            // putchar('\n');  // Move to the next line after printing all bits

        packet_info->dns_record_struct.OP_code[0] = ((dns_flags[0] >> 3) & 1)  + '0';    //    .... X...
        packet_info->dns_record_struct.OP_code[1] = ((dns_flags[0] >> 4) & 1)  + '0';    //    ...X ....
        packet_info->dns_record_struct.OP_code[2] = '\0';

        //0...a standard query (QUERY)
        if (packet_info->dns_record_struct.OP_code[0] == '0' && packet_info->dns_record_struct.OP_code[1] == '0')
        {
            packet_info->dns_record_struct.OP_code[0] = '0';
            packet_info->dns_record_struct.OP_code[1] = '\0';
        }
        //0...a standard query (QUERY)
        else if (packet_info->dns_record_struct.OP_code[0] == '1' && packet_info->dns_record_struct.OP_code[1] == '0')
        {
            packet_info->dns_record_struct.OP_code[0] = '1';
            packet_info->dns_record_struct.OP_code[1] = '\0';
        }
        //0...a standard query (QUERY)
        else if (packet_info->dns_record_struct.OP_code[0] == '0' && packet_info->dns_record_struct.OP_code[1] == '1')
        {
            packet_info->dns_record_struct.OP_code[0] = '2';
            packet_info->dns_record_struct.OP_code[1] = '\0';
        }
        else
        {
            packet_info->dns_record_struct.OP_code[0] = 'R';    //3-15...reserved for future use
            packet_info->dns_record_struct.OP_code[1] = '\0';
        }
    // AA record
        packet_info->dns_record_struct.AA[0] = ((dns_flags[0] >> 2) & 1)  + '0';    //    .... .X..
        packet_info->dns_record_struct.AA[1] = '\0';
    //TC record
        packet_info->dns_record_struct.TC[0] = ((dns_flags[0] >> 1) & 1)  + '0';    //    .... ..X.
        packet_info->dns_record_struct.TC[1] = '\0';
    //RD
        packet_info->dns_record_struct.RD[0] = ((dns_flags[0] >> 0) & 1)  + '0';    //    .... ...X
        packet_info->dns_record_struct.RD[1] = '\0';
    //RA
        packet_info->dns_record_struct.RA[0] = ((dns_flags[1] >> 7) & 1)  + '0';    //    X... .... 2nd flag byte  
        packet_info->dns_record_struct.RA[1] = '\0';
    //Z
        packet_info->dns_record_struct.Z[0] = ((dns_flags[1] >> 6) & 1)  + '0';    //    .X.. .... 2nd flag byte  
        packet_info->dns_record_struct.Z[1] = '\0';
    //AD
        packet_info->dns_record_struct.AD[0] = ((dns_flags[1] >> 5) & 1)  + '0';    //    ..X. .... 2nd flag byte  
        packet_info->dns_record_struct.AD[1] = '\0';
    //CD
        packet_info->dns_record_struct.CD[0] = ((dns_flags[1] >> 4) & 1)  + '0';    //    ...X .... 2nd flag byte  
        packet_info->dns_record_struct.CD[1] = '\0';
    //Rcode
        char Rcode_string [5];
        Rcode_string [0] = ((dns_flags[1] >> 3) & 1)  + '0';    //    .... X...
        Rcode_string [1] = ((dns_flags[1] >> 2) & 1)  + '0';    //    .... .X..
        Rcode_string [2] = ((dns_flags[1] >> 1) & 1)  + '0';    //    .... ..X.
        Rcode_string [3] = ((dns_flags[1] >> 0) & 1)  + '0';    //    .... ...X
        Rcode_string [4] = '\0';

        if (strcmp (Rcode_string, "0000") == 0)  // 0 No error
        {
            packet_info->dns_record_struct.RCODE[0] = '0';
        }
        else if (strcmp (Rcode_string, "0001") == 0) // 1 Format error
        {
            packet_info->dns_record_struct.RCODE[0] = '1';
        }
        else if (strcmp (Rcode_string, "0010") == 0) // 2 Server failure
        {
            packet_info->dns_record_struct.RCODE[0] = '2';
        }
        else if (strcmp (Rcode_string, "0011") == 0) // 3 Name error
        {
            packet_info->dns_record_struct.RCODE[0] = '3';
        }
        else if (strcmp (Rcode_string, "0100") == 0) // 4 Not implemented
        {
            packet_info->dns_record_struct.RCODE[0] = '4';
        }
        else if (strcmp (Rcode_string, "0101 ") == 0) // 5 Refused
        {
            packet_info->dns_record_struct.RCODE[0] = '5';
        }
        else    // Reserved for future use
        {
            packet_info->dns_record_struct.RCODE[0] = 'R';
        }
        packet_info->dns_record_struct.RCODE[1] = '\0';
        
        
}

void get_dns_header (struct_packet_info *packet_info){
    if (packet_info == NULL)
    {
        fprintf (stderr, "Error function get_dns_header received NULL ptr\n");
        return;
    }
    u_int32_t dns_offset = 0;
    u_char * actual_dns_bits = NULL;

    // printf("total_packet_length je: %u\n", packet_info->packet_lengths.total_packet_length);
    // printf("udp_length je: %u\n", packet_info->packet_lengths.udp_length);

    dns_offset = (packet_info->packet_lengths.total_packet_length) - (packet_info->packet_lengths.udp_length);
    
    actual_dns_bits = (u_char *) packet_raw + dns_offset ;
    packet_info->packet_lengths.dns_header_ptr = actual_dns_bits;

    // printf("DNS offset je: %u\n", dns_offset);  //Debug
    // printf("DNS hlavicka zacina: %02x %02x %02x\n",  //Debug
    // *(unsigned char *)(actual_dns_bits), 
    // *(unsigned char *)(actual_dns_bits+1), *(unsigned char *)(actual_dns_bits+2));

    //Fill Identifier
    if (actual_dns_bits != NULL && (actual_dns_bits + 1) != NULL)
    {
        packet_info->dns_record_struct.Identifier[0] = *(unsigned char *) actual_dns_bits;
        packet_info->dns_record_struct.Identifier[1] = *(unsigned char *) (actual_dns_bits + 1);
    }
    else
    {
        fprintf (stderr, "Error function get_dns_header received actual_dns_bits NULL ptr\n");
        return;
    }
    
        
        packet_info->dns_record_struct.Identifier[2] = '\0';

    fill_dns_flags(packet_info);

    // printf("Message is %s\n", packet_info->dns_record_struct.QR_record);    //Debug
    // printf("OPcode is: %s\n", packet_info->dns_record_struct.OP_code);      //Debug
    // printf("AA is: %s\n", packet_info->dns_record_struct.AA);       //Debug
    // printf("TC is: %s\n", packet_info->dns_record_struct.TC);       //Debug
    // printf("RD is: %s\n", packet_info->dns_record_struct.RD);       //Debug
    // printf("RA is: %s\n", packet_info->dns_record_struct.RA);       //Debug
    // printf("TC is: %s\n", packet_info->dns_record_struct.TC);       //Debug
    // printf("Z is: %s\n", packet_info->dns_record_struct.Z);         //Debug
    // printf("AD is: %s\n", packet_info->dns_record_struct.AD);       //Debug
    // printf("CD is: %s\n", packet_info->dns_record_struct.CD);       //Debug
    // printf("Rcode is: %s\n", packet_info->dns_record_struct.RCODE);      //Debug
    


}

unsigned int u_CharArr_2_u_Int(const unsigned char *byteArray) {
    unsigned int result;
    result = (unsigned int)(byteArray[0]) << 8 | 
             (unsigned int)(byteArray[1]);
    return result;
}

void fill_QAAA(struct_packet_info *packet_info){
    unsigned char array [3];
    array [2] = '\0';

    //Get Pointer to "Question" part
    u_char * Question = packet_info->packet_lengths.dns_header_ptr + 4 ;
        array [0] = *(unsigned char *)(Question);
        array [1] = *(unsigned char *)(Question+1);
        packet_info->dns_record_struct.NoQuestions = u_CharArr_2_u_Int(array);

    //Get Pointer to "Answer" part
    u_char * Answer = packet_info->packet_lengths.dns_header_ptr + 6 ;
        array [0] = *(unsigned char *)(Answer);
        array [1] = *(unsigned char *)(Answer+1);
        packet_info->dns_record_struct.NoAnswers = u_CharArr_2_u_Int(array);

    //Get Pointer to "Authority" part
    u_char * Authority = packet_info->packet_lengths.dns_header_ptr + 8 ;
        array [0] = *(unsigned char *)(Authority);
        array [1] = *(unsigned char *)(Authority+1);
        packet_info->dns_record_struct.NoAuthoritys = u_CharArr_2_u_Int(array);

    //Get Pointer to "Additional" part
    u_char * Additional = packet_info->packet_lengths.dns_header_ptr + 10 ;
        array [0] = *(unsigned char *)(Additional);
        array [1] = *(unsigned char *)(Additional+1);
        packet_info->dns_record_struct.NoAdditions = u_CharArr_2_u_Int(array);
}

void print_info (const struct_packet_info *packet_info){
    if (packet_info == NULL)
    {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        fprintf(stderr, "Error, function print_info() received NULL prt\n");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
    }
    
    char time [100];
        strncpy(time, packet_info->time_info, 99);
    if (global_argument_struct_ptr->verbose == false)
    {
        printf("%s", time);
        printf(" ");
        printf("%s", packet_info->src_ip);
        printf (" -> ");
        printf("%s", packet_info->dst_ip);
        printf (" (");
        printf("%c ", packet_info->dns_record_struct.QR_record[0]);
        printf("%u/", packet_info->dns_record_struct.NoQuestions);
        printf("%u/", packet_info->dns_record_struct.NoAnswers);
        printf("%u/", packet_info->dns_record_struct.NoAuthoritys);
        printf("%u)", packet_info->dns_record_struct.NoAdditions);

        printf("\n");
    }
    else
    {
        // printf("Paket cislo: %d\n", GLOBAL_number_of_packets);
        GLOBAL_number_of_packets++;
        // if (GLOBAL_number_of_packets > 10)
        // {
        //     exit(0);
        // }
        
        printf("Timestamp: ");
            printf("%s", time);
            printf("\n");
        printf("SrcIP: ");
            printf("%s\n", packet_info->src_ip);
        printf("DstIP: ");
            printf("%s\n", packet_info->dst_ip);
        printf("SrcPort: ");
            printf("%d\n", packet_info->src_port);
        printf("DstPort: ");
            printf("%d\n", packet_info->dst_port);
        printf("Identifier: ");
            printf("0x%02X%02X\n", packet_info->dns_record_struct.Identifier[0], packet_info->dns_record_struct.Identifier[1]);
        printf("Flags: ");
            printf("QR=");
                if (packet_info->dns_record_struct.QR_record[0] == 'Q')
                {
                    printf("0,");
                }
                else
                {
                    printf("1,");
                }
            printf("OPCODE=");
                printf("%s, ", packet_info->dns_record_struct.OP_code);
            printf("AA=");
                printf("%s, ", packet_info->dns_record_struct.AA);
            printf("TC=");
                printf("%s, ", packet_info->dns_record_struct.TC);
            printf("RD=");
                printf("%s, ", packet_info->dns_record_struct.RD);
            printf("RA=");
                printf("%s, ", packet_info->dns_record_struct.RA);
            printf("AD=");
                printf("%s, ", packet_info->dns_record_struct.AD);
            printf("CD=");
                printf("%s, ", packet_info->dns_record_struct.CD);
            printf("RCODE=");
                printf("%s, ", packet_info->dns_record_struct.RCODE);
            printf("\n");
        printf("\n");
        
    }
    
}

void fill_packet_info (struct_packet_info *packet_info, const struct pcap_pkthdr *packet_header,struct ip* ip_header, const u_char *packet_ptr){
    print_timestamp(packet_info);  //Done
    print_ip_address(ip_header, packet_info); //Done
    fill_frame_length(packet_header, packet_info);
    print_port_number(ip_header, packet_ptr, packet_info); //Done
    get_dns_header(packet_info);  
    fill_QAAA(packet_info);
}

void packet_handler(u_char *user,const struct pcap_pkthdr *packet_header, const u_char *packet_ptr)
{
    // struct ether_header *eth_header;
    struct ip* ip_header;

    //Create packet_info struct
    struct_packet_info packet_info;
    init_packet_info(&packet_info);
        packet_info.packet_lengths.link_header = link_header_length;



    packet_raw = (u_char *)malloc((packet_header->caplen)+1);
    if (packet_raw == NULL) {
        fprintf (stderr, "Error while allocating memory\n");
        return;
    }

    // Copy packet_ptr data to packet_raw free(packet_raw);
    memcpy(packet_raw, packet_ptr, packet_header->caplen);
 
     // Skip the datalink layer header and get the IP header fields.
    packet_ptr += link_header_length;
    ip_header = (struct ip*)packet_ptr;

    // eth_header = (struct ether_header *)packet_ptr;

    // uint16_t ethertype = ntohs(((struct ether_header*)packet_raw)->ether_type);

    if (global_argument_struct_ptr == NULL)
    {
        fprintf(stderr, "Error, packet_handler function recived NULL ptr to global_argument_struct\n");
    }

    fill_packet_info(&packet_info, packet_header, ip_header, packet_ptr );
    print_info (&packet_info);
    fill_non_header_dns_parts(&packet_info, packet_raw, global_verbose_set);

        switch (ip_header->ip_p)
        {
            case IPPROTO_UDP:
                if (global_argument_struct_ptr->verbose == false)
            {
                
            }
                
                // printf("UDP\n");
                // print_MAC_addresses(eth_header);    
                // fill_frame_length(packet_header, &packet_info);
                // print_ip_address(ip_header, &packet_info);
                // print_port_number(ip_header, packet_ptr, &packet_info); //Done
                // print_packet_data(packet_header);
                // printf("\n");
                break;


            default:
                printf("Unknown protocol\n");
            break;
        }

    

    free (packet_raw);
    packet_raw = NULL;
    

    printf("\n");
    
}

/**
 * End of file libcap_engine.c
 */
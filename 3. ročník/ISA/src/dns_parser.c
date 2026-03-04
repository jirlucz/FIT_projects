/**
 * File: dns_parser.c
 * Author: Jiri Kotek
 * Email: xkotek09@stud.fit.vutbr.cz
 * Date: 15. 11. 2024
 */

#include "main.h"

#define DNS_HEADER_LENGTH 12
#define NAME_GIVEN_BY_PTR 2

question_section *question_section_array = NULL;
resource_record_section *answer_section_array = NULL;
resource_record_section *authority_section_array = NULL;
resource_record_section *additional_section_array = NULL;

u_int16_t GLOBAL_no_question_section = 0;
u_int16_t GLOBAL_no_answer_section = 0;
u_int16_t GLOBAL_no_authority_section = 0;
u_int16_t GLOBAL_no_additional_section = 0;

bool global_verbose = false;


void convert_ipv6_to_str(const unsigned char *ipv6_bytes, char *ipv6_str) {
    if (inet_ntop(AF_INET6, ipv6_bytes, ipv6_str, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop failed");
    }
}

void print_dns_class (u_char class_arr[3]){
    if (class_arr[0] == 0x00 && class_arr[1] == 0x01)
    {
        printf("IN ");
    }
    else if (class_arr[0] == 0x00 && class_arr[1] == 0x02)
    {
        printf("CS ");
    }
    else if (class_arr[0] == 0x00 && class_arr[1] == 0x03)
    {
        printf("CH ");
    }
    else if (class_arr[0] == 0x00 && class_arr[1] == 0x02)
    {
        printf("HS ");
    }
    else if (class_arr[0] == 0xFF && class_arr[1] == 0xFF)
    {
        printf("* ");
    }
    else
    {
        printf("UNKNOWN ");
    }
}

void print_dns_type(u_char type_arr[3]){
    if (type_arr[0] == 0x00 && type_arr[1] == 0x01)
    {
        printf("A ");
    }

    else if (type_arr[0] == 0x00 && type_arr[1] == 0x02)
    {
        printf("NS ");
    }

    else if (type_arr[0] == 0x00 && type_arr[1] == 0x0f)
    {
        printf("MX ");
    }

    else if (type_arr[0] == 0x00 && type_arr[1] == 0x1c)
    {
        printf("AAAA ");
    }

    else if (type_arr[0] == 0x00 && type_arr[1] == 0x05)
    {
        printf("CNAME ");
    }

    else if (type_arr[0] == 0x00 && type_arr[1] == 0x06)
    {
        printf("SOA ");
    }

    else if (type_arr[0] == 0x00 && type_arr[1] == 0x21)
    {
        printf("SRV ");
    }

    else
    {
        printf("UNKNOWN ");
    }
}

void print_verbose_dns_data (struct_packet_info* packet_info, bool verbose){

    if (verbose == false)
    {
        return;
    }
    
    printf("[Question Section]\n");
            if (packet_info->dns_non_header_struct.arr_questions != NULL && packet_info->dns_record_struct.NoQuestions != 0)
            {
                for (int i = 0; i < packet_info->dns_record_struct.NoQuestions; i++)
                {
                    //QNAME part
                    printf("%s. ", packet_info->dns_non_header_struct.arr_questions[i].QNAME);
                    // QCLASS part
                    print_dns_class(packet_info->dns_non_header_struct.arr_questions[i].QCLASS);
                    // QTYPE part
                    print_dns_type(packet_info->dns_non_header_struct.arr_questions[i].QTYPE);
                }
            }
            
            

        printf("\n");
        printf("\n");
        printf("[Answer Section]\n");
            if (packet_info->dns_non_header_struct.arr_answers != NULL && packet_info->dns_record_struct.NoAnswers != 0)
            {
                for (int i = 0; i < packet_info->dns_record_struct.NoAnswers; i++)
                {
                        //NAME part
                        printf("%s. ", packet_info->dns_non_header_struct.arr_answers[i].NAME);
                        // TTL
                        printf(" %u ", (packet_info->dns_non_header_struct.arr_answers[i].TTL));
                        // CLASS part
                        print_dns_class(packet_info->dns_non_header_struct.arr_answers[i].CLASS);
                        // TYPE part
                        print_dns_type(packet_info->dns_non_header_struct.arr_answers[i].TYPE);

                        if (packet_info->dns_non_header_struct.arr_answers[i].TYPE[1] == 0x06)
                        {
                            printf("%s. \t", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.M_NAME);
                            printf("%s. ", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.R_NAME);
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.SERIAL);
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.REFRESH);
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.RETRY);
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.EXPIRE);
                            printf("%u", packet_info->dns_non_header_struct.arr_answers[i].SOA_record.MAX_TTL);
                        }
                        else if (packet_info->dns_non_header_struct.arr_answers[i].TYPE[1] == 0x0f)
                        {
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].MX_record.PREFERENCE);
                            printf("%s", packet_info->dns_non_header_struct.arr_answers[i].MX_record.EXCHANGE);
                            printf(".");
                        }
                        else if (packet_info->dns_non_header_struct.arr_answers[i].TYPE[1] == 0x21)
                        {
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SRV_record.PRIORITY);
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SRV_record.WEIGHT);
                            printf("%u ", packet_info->dns_non_header_struct.arr_answers[i].SRV_record.PORT);
                            printf("%s.", packet_info->dns_non_header_struct.arr_answers[i].SRV_record.NAME);
                        }
                        else if (packet_info->dns_non_header_struct.arr_answers[i].TYPE[1] == 0x01)
                        {
                            printf("%s", packet_info->dns_non_header_struct.arr_answers[i].RDATA);
                        }
                        else if (packet_info->dns_non_header_struct.arr_answers[i].TYPE[1] == 0x1c)
                        {
                            printf("%s", packet_info->dns_non_header_struct.arr_answers[i].RDATA);
                        }
                        else
                        {
                            printf("%s.", packet_info->dns_non_header_struct.arr_answers[i].RDATA);
                        }

                        printf("\n");    
                }
            }

        printf("\n");
        printf("[Authority Section]\n");
            if (packet_info->dns_non_header_struct.arr_authorities != NULL && packet_info->dns_record_struct.NoAuthoritys != 0)
            {
                for (int i = 0; i < packet_info->dns_record_struct.NoAuthoritys; i++)
                {
                    //NAME part
                    printf("%s. ", packet_info->dns_non_header_struct.arr_authorities[i].NAME);
                    // TTL
                    printf(" %u ", (packet_info->dns_non_header_struct.arr_authorities[i].TTL));
                    // CLASS part
                    print_dns_class(packet_info->dns_non_header_struct.arr_authorities[i].CLASS);
                    // TYPE part
                    print_dns_type(packet_info->dns_non_header_struct.arr_authorities[i].TYPE);

                    if (packet_info->dns_non_header_struct.arr_authorities[i].TYPE[1] == 0x06)
                    {
                        printf("%s. \t", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.M_NAME);
                        printf("%s. ", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.R_NAME);
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.SERIAL);
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.REFRESH);
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.RETRY);
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.EXPIRE);
                        printf("%u", packet_info->dns_non_header_struct.arr_authorities[i].SOA_record.MAX_TTL);
                    }
                    else if (packet_info->dns_non_header_struct.arr_authorities[i].TYPE[1] == 0x0f)
                    {
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].MX_record.PREFERENCE);
                        printf("%s", packet_info->dns_non_header_struct.arr_authorities[i].MX_record.EXCHANGE);
                    }

                    else if (packet_info->dns_non_header_struct.arr_authorities[i].TYPE[1] == 0x21)
                    {
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SRV_record.PRIORITY);
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SRV_record.WEIGHT);
                        printf("%u ", packet_info->dns_non_header_struct.arr_authorities[i].SRV_record.PORT);
                        printf("%s.", packet_info->dns_non_header_struct.arr_authorities[i].SRV_record.NAME);
                    }
                    else if (packet_info->dns_non_header_struct.arr_authorities[i].TYPE[1] == 0x01)
                    {
                        printf("%s", packet_info->dns_non_header_struct.arr_authorities[i].RDATA);
                    }
                    else if (packet_info->dns_non_header_struct.arr_authorities[i].TYPE[1] == 0x1c)
                    {
                        printf("%s", packet_info->dns_non_header_struct.arr_authorities[i].RDATA);
                    }
                    else
                    {
                        printf("%s.", packet_info->dns_non_header_struct.arr_authorities[i].RDATA);
                    }

                    printf("\n");    
                }
            }

        printf("\n");
        printf("[Additional Section]\n");
             if (packet_info->dns_non_header_struct.arr_additional != NULL && packet_info->dns_record_struct.NoAdditions != 0)
            {
                for (int i = 0; i < packet_info->dns_record_struct.NoAdditions; i++)
                {
                        //NAME part
                        printf("%s. ", packet_info->dns_non_header_struct.arr_additional[i].NAME);
                        // TTL
                        printf(" %u ", (packet_info->dns_non_header_struct.arr_additional[i].TTL));
                        // CLASS part
                        print_dns_class(packet_info->dns_non_header_struct.arr_additional[i].CLASS);
                        // TYPE part
                        print_dns_type(packet_info->dns_non_header_struct.arr_additional[i].TYPE);

                        if (packet_info->dns_non_header_struct.arr_additional[i].TYPE[1] == 0x06)
                        {
                            printf("%s. \t", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.M_NAME);
                            printf("%s. ", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.R_NAME);
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.SERIAL);
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.REFRESH);
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.RETRY);
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.EXPIRE);
                            printf("%u", packet_info->dns_non_header_struct.arr_additional[i].SOA_record.MAX_TTL);
                            
                        }
                        else if (packet_info->dns_non_header_struct.arr_additional[i].TYPE[1] == 0x0f)
                        {
                            printf("%s", packet_info->dns_non_header_struct.arr_additional[i].MX_record.EXCHANGE);
                        }
                        else if (packet_info->dns_non_header_struct.arr_additional[i].TYPE[1] == 0x21)
                        {
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SRV_record.PRIORITY);
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SRV_record.WEIGHT);
                            printf("%u ", packet_info->dns_non_header_struct.arr_additional[i].SRV_record.PORT);
                            printf("%s.", packet_info->dns_non_header_struct.arr_additional[i].SRV_record.NAME);
                        }
                        else if (packet_info->dns_non_header_struct.arr_additional[i].TYPE[1] == 0x01)
                        {
                            printf("%s", packet_info->dns_non_header_struct.arr_additional[i].RDATA);
                        }
                        else if (packet_info->dns_non_header_struct.arr_additional[i].TYPE[1] == 0x1c)
                        {
                            printf("%s", packet_info->dns_non_header_struct.arr_additional[i].RDATA);
                        }
                        else
                        {
                            printf("%s.", packet_info->dns_non_header_struct.arr_additional[i].RDATA);
                        }

                        printf("\n");    
                }
            }

                
        printf("\n"); 

    return;
}

void dns_parser_clean(){
    if (question_section_array != NULL)
    {
        for (int i=0; i<GLOBAL_no_question_section; i++){
            if (question_section_array[i].QNAME != NULL)
            {
                free(question_section_array[i].QNAME);
            }
        }
        
        
        free(question_section_array);
        question_section_array = NULL;
    }

    if (answer_section_array != NULL)
    {
        for (int i=0; i<GLOBAL_no_answer_section; i++){
            free(answer_section_array[i].RDATA);
        }

        free(answer_section_array);
        answer_section_array = NULL;
    }

    if (authority_section_array != NULL)
    {
        for (int i=0; i<GLOBAL_no_authority_section; i++){
            free(authority_section_array[i].RDATA);
        }
        free(authority_section_array);
        authority_section_array = NULL;
    }

    if (additional_section_array != NULL)
    {
        for (int i=0; i<GLOBAL_no_additional_section; i++){
            free(additional_section_array[i].RDATA);
        }
        free(additional_section_array);
        additional_section_array = NULL; 
    }
    
}

void dns_parser_setup(struct_packet_info* packet_info, u_char *packet_raw){
    if (packet_info == NULL || packet_raw == NULL)
    {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        fprintf(stderr, "Error, received NULL pointer in parse_question_section() in file dns_parser.c\n");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        return;
    }
    
    u_int16_t question_count = packet_info->dns_record_struct.NoQuestions;
    u_int16_t answers_count = packet_info->dns_record_struct.NoAnswers;
    u_int16_t authorities_count = packet_info->dns_record_struct.NoAuthoritys;
    u_int16_t additional_count = packet_info->dns_record_struct.NoAdditions;

    question_section_array = NULL;
    answer_section_array = NULL;
    authority_section_array = NULL;
    additional_section_array = NULL;

    GLOBAL_no_question_section = packet_info->dns_record_struct.NoQuestions;
    GLOBAL_no_answer_section = packet_info->dns_record_struct.NoAnswers;
    GLOBAL_no_authority_section = packet_info->dns_record_struct.NoAuthoritys;
    GLOBAL_no_additional_section = packet_info->dns_record_struct.NoAdditions;

    if (question_count != 0)
    {
        question_section_array = (question_section *)malloc(question_count * sizeof(question_section));
        if (question_section_array == NULL)
        {
            fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
            fprintf(stderr, "Error, malloc returned NULL pointer in function dns_parser_setup in file dns_parser.c\n");
            fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
            return;
        }
    }

    if (answers_count != 0)
    {
        answer_section_array =  (resource_record_section *)malloc(answers_count * sizeof(resource_record_section));
        if (answer_section_array == NULL)
        {
            fprintf(stderr, "Error, malloc returned NULL pointer in function dns_parser_setup in file dns_parser.c\n");
            return;
        }
    }
    
    if (authorities_count != 0)
    {
        authority_section_array = (resource_record_section *)malloc(authorities_count * sizeof(resource_record_section));
        if (authority_section_array == NULL)
        {
            fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
            fprintf(stderr, "Error, malloc returned NULL pointer in function dns_parser_setup in file dns_parser.c\n");
            fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
            return;
        }
    }

    if (additional_count != 0)
    {
        additional_section_array = (resource_record_section *)malloc(additional_count * sizeof(resource_record_section));
        if (additional_section_array == NULL)
        {
            fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
            fprintf(stderr, "Error, malloc returned NULL pointer in function dns_parser_setup in file dns_parser.c\n");
            fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
            return;
        }
    }

    for (int i = 0; i < question_count; i++)
    {
        for (int j = 0; j < 3; j++) {
            question_section_array[i].QCLASS[j] = '\0';
            question_section_array[i].QTYPE[j] = '\0';
        }
        question_section_array[i].end_question_ptr = NULL;
        question_section_array[i].QNAME = NULL;
    }
    packet_info->dns_non_header_struct.arr_answers = answer_section_array;

    for (int i = 0; i < answers_count; i++)
    {
        answer_section_array[i].type_of_record = Answer;
        for (int j = 0; j < 3; j++) {
            answer_section_array[i].CLASS[j] = '\0';
            answer_section_array[i].TYPE[j] = '\0';
        }
        answer_section_array[i].end_record_ptr = NULL;
        // answer_section_array[i].NAME = NULL;
        answer_section_array[i].RDATA = NULL;
        
    }
    packet_info->dns_non_header_struct.arr_answers = answer_section_array;

    
    for (int i = 0; i < authorities_count; i++)
    {
        authority_section_array[i].type_of_record = Authority;
        for (int j = 0; j < 3; j++) {
            authority_section_array[i].CLASS[j] = '\0';
            authority_section_array[i].TYPE[j] = '\0';
        }
        authority_section_array[i].end_record_ptr = NULL;
        // authority_section_array[i].NAME = NULL;
        authority_section_array[i].RDATA = NULL;
        authority_section_array[i].SOA_record.M_NAME[0] = '\0';
        authority_section_array[i].SOA_record.R_NAME[0] = '\0';
    }
    packet_info->dns_non_header_struct.arr_authorities = authority_section_array;


    for (int i = 0; i < additional_count; i++)
    {
        additional_section_array[i].type_of_record = Additional;
        for (int j = 0; j < 3; j++) {
            additional_section_array[i].CLASS[j] = '\0';
            additional_section_array[i].TYPE[j] = '\0';
        }
        additional_section_array[i].end_record_ptr = NULL;
        // additional_section_array[i].NAME = NULL;
        additional_section_array[i].RDATA = NULL;
    }
    packet_info->dns_non_header_struct.arr_additional = additional_section_array;

    packet_info->dns_non_header_struct.arr_questions = question_section_array;
    packet_info->dns_non_header_struct.arr_answers = answer_section_array;
    packet_info->dns_non_header_struct.arr_authorities = authority_section_array;
    packet_info->dns_non_header_struct.arr_additional = additional_section_array;

    return;
}

int32_t u_CharArr_4_Int32(u_char *arr){
    if (arr == NULL)
    {
        return 0;
    }
    
    int32_t int_result = (int32_t)(arr[0] << 24) |
                         (int32_t)(arr[1] << 16) |
                         (int32_t)(arr[2] << 8) |
                         (int32_t)(arr[3]);
    
    // The 5th byte is ignored 
    // Check if the result is negative (sign bit)
    if (int_result & 0x80000000) {  // Check if the sign bit is set
        int_result |= 0xFFFFFFFF;    // Adjust for negative value
    }
    
    return int_result;
}

uint16_t u_CharArr_2_u_Int16(const unsigned char *byteArray) {
    uint16_t result;
    result = (uint16_t)(byteArray[0]) << 8 | 
             (uint16_t)(byteArray[1]);
    return result;
}

uint16_t load_domain_name (u_char *destination,const u_char *source, uint16_t name_length, u_char *packet_raw, struct_packet_info *packet_info, bool save_to_file){

    if (destination == NULL || source == NULL || packet_raw == NULL || packet_info == NULL)
    {
        fprintf (stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n ");
        fprintf (stderr, "Error in function load_domain_name(), function received NULL pointer\n ");
        fprintf (stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n ");
    }
    
    const u_char *actual_char = source + 1;  // Move past the first length byte

    uint16_t index_string_w_dots = 0;       // Index to insert characters in the destination
    uint16_t raw_length_of_the_name = 1;   // Length of name segment in packet
    uint16_t segment_length = *source;       // Length of the first segment
    
    const u_char *end = packet_raw + packet_info->packet_lengths.total_packet_length;  // End of the packet
    while (segment_length > 0 && actual_char < end) {
        // If the segment starts with a pointer (first two bits are 1)
        if ((segment_length & 0xC0) == 0xC0) 
        {
            raw_length_of_the_name += 2;
            u_char index_arr [3];
            u_char bit_mask = 0x3F ; // 0b 0011 1111
            u_int16_t name_len_before = 0;
            u_int16_t name_len_after = 0;

            index_arr [0]= *(actual_char-1);
                index_arr[0] &= bit_mask;
            index_arr [1]= *(actual_char);
            index_arr [2]= '\0';

            // Pointer: the next two bytes represent a pointer to another part of the domain name
            u_int16_t pointer_offset = u_CharArr_2_u_Int16(index_arr);
            // printf("Offset je: %u\n", pointer_offset); //Debug
            destination[index_string_w_dots ] = '\0';  
            name_len_before = strlen ((char *) destination);
            load_domain_name(&destination[index_string_w_dots], (packet_info->packet_lengths.dns_header_ptr + pointer_offset), 0,packet_raw, packet_info, save_to_file);
            name_len_after = strlen ((char *) destination);
            // printf("Prubezne domain name: %s\n", destination);   // Debug
            // printf("Jeho delka je: %u\n", name_len_after-name_len_before); // Debug
            actual_char += 2;  // Move past the pointer
            index_string_w_dots += name_len_after-name_len_before+1;
        } 

        else 
        {
            raw_length_of_the_name++;
            raw_length_of_the_name += segment_length;
            // Normal segment (label): copy the segment to the destination
            for (int i = 0; i < segment_length; i++) {
                // printf ("%c", *actual_char);
                destination[index_string_w_dots] = *actual_char;
                if (actual_char == end)
                {
                    break;
                }
                

                index_string_w_dots++;
                actual_char++;
            }
            // printf("\n");

            // After copying a segment, if there is another segment, add a dot.
            if (actual_char != end && *actual_char != 0x00) {
                destination[index_string_w_dots++] = '.';
            }
        }


        if (actual_char == end)
        {
            raw_length_of_the_name -= segment_length+2;
            break;
        }
        segment_length = *actual_char++;  // Move to the next segment set_bool_name_ptrlength
    }
    // Null-terminate the destination string
    destination[index_string_w_dots] = '\0';

    if (save_to_file == true)
    {
        write_to_file(destination, NULL);
    }
    // printf("Final domain name: %s\n", destination); // Debug
    return raw_length_of_the_name;
}

uint16_t name_length(const u_char *string, struct_packet_info *packet_info, u_char *packet_raw, bool *set_bool_name_ptr, u_int16_t *set_offset){
    if (string == NULL || packet_info == NULL || packet_raw == NULL ) {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        fprintf(stderr, "Error, received NULL pointer in name_length()");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        return 0;
    }

    u_int16_t length = 0;
    const u_char *actual_char = NULL; 
    const u_char *end = NULL;

    actual_char = string; 
    end = packet_raw + packet_info->packet_lengths.total_packet_length; // Calculate end address

    u_char first_byte = '\0';
    first_byte = *string;
    u_char pointer_offset_array [3];
    // printf("First byte are %0x %0x %0x %0x\n", first_byte, *(string+1), *(string+2), *(string+3));  //Debug
    
    int first_bit = 0;
    int second_bit = 0;
    // Extract pointer bits
    first_bit = (first_byte >> 7) & 1; // Check the first bit
    second_bit = (first_byte >> 6) & 1; // Check the second bit

    // name is given by pointer
    if (first_bit == 1 && second_bit == 1)
    {
        if (set_bool_name_ptr != NULL)
        {
            *set_bool_name_ptr = true;
        }
        
        // Remove ptr flags and calculate the offset of the name
        u_int16_t offset_number = 0;
        u_char bit_mask = 0x3F ; // 0b 0011 1111
        pointer_offset_array[0] = *(actual_char);   //MSB
        pointer_offset_array[1] = *(actual_char+1); //LSB

        pointer_offset_array[0] &= bit_mask;

        // printf("pointer_offset_array po masce je %0x %0x\n", pointer_offset_array[0], pointer_offset_array[1]); //Debug

        offset_number = u_CharArr_2_u_Int16(pointer_offset_array);
        if (set_offset != NULL)
        {
            *set_offset = offset_number;
        }      
        // printf ("Offset number is: %u\n", offset_number);   //Debug
    }
    else
    {
        if (set_bool_name_ptr != NULL)
        {
            *set_bool_name_ptr = false;
        }
    }

    while (*actual_char != 0x00 && actual_char != end) {
            actual_char++;
            length++;
        }

    return (length-1);
}

void parse_MX(struct_packet_info* packet_info, MX_record_struct* destination, u_char* position, u_char *packet_raw){
    destination->PREFERENCE = 0;
    u_char array [2];
        for (int i = 0; i < 2; i++)
        {
            array[i] = *(position+i);
        }
    destination->PREFERENCE = u_CharArr_2_u_Int16(array);

    load_domain_name(destination->EXCHANGE, position+2, 0, packet_raw, packet_info, true);

    return;
} 

void parse_SRV(struct_packet_info* packet_info, SRV_record_struct* destination, u_char* position, u_char *packet_raw){
    destination->PORT = 0;
    destination->PRIORITY = 0;
    destination->WEIGHT = 0;

    // load MNAME
    // name_len = load_domain_name(destination->TARGET, position, 0, packet_raw, packet_info);
    // +2
    u_char array [2];
        for (int i = 0; i < 2; i++)
        {
            array[i] = *(position+i);
        }
    destination->PRIORITY = u_CharArr_2_u_Int16(array);

    position+=2;
    for (int i = 0; i < 2; i++)
        {
            array[i] = *(position+i);
        }
    destination->WEIGHT = u_CharArr_2_u_Int16(array);

    position+=2;
    for (int i = 0; i < 2; i++)
        {
            array[i] = *(position+i);
        }
    destination->PORT = u_CharArr_2_u_Int16(array);

    load_domain_name(destination->NAME, position+2, 0, packet_raw, packet_info, true);
    return;
} 

void parse_SOA(struct_packet_info* packet_info, SOA_record_struct* destination, u_char* position, u_char *packet_raw){
    uint16_t name_len = 0;
    // load MNAME
    name_len = load_domain_name(destination->M_NAME, position, 0, packet_raw, packet_info, true);
    // load RNAME
    name_len += load_domain_name(destination->R_NAME, position+name_len, 0, packet_raw, packet_info, false);

    u_char array [4];
        for (int i = 0; i < 4; i++)
        {
            array[i] = *(position+name_len+i);
        }
    destination->SERIAL = u_CharArr_4_Int32(array);

    // Get refresh
    position += 4;
        for (int i = 0; i < 4; i++)
        {
            array[i] = *(position+name_len+i);
        }
    destination->REFRESH = u_CharArr_4_Int32(array);

    // Get retry
    position += 4;
        for (int i = 0; i < 4; i++)
        {
            array[i] = *(position+name_len+i);
        }
    destination->RETRY = u_CharArr_4_Int32(array);

    // Get expire
    position += 4;
        for (int i = 0; i < 4; i++)
        {
            array[i] = *(position+name_len+i);
        }
    destination->EXPIRE = u_CharArr_4_Int32(array);

    // Get max_TTL
    position += 4;
        for (int i = 0; i < 4; i++)
        {
            array[i] = *(position+name_len+i);
        }
    destination->MAX_TTL = u_CharArr_4_Int32(array);

    return;
}

u_char* parse_question_section(struct_packet_info* packet_info, u_char *packet_raw){
    if (packet_info == NULL || packet_raw == NULL)
    {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        fprintf(stderr, "Error, received NULL pointer in parse_question_section()1");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        return NULL;
    }

    if (packet_info->dns_non_header_struct.arr_questions == NULL )
    {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        fprintf(stderr, "Error, received NULL arr_questions pointer in parse_question_section()2");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        return NULL;
    }
    
    // offset_with_multiple_records defined offset of each question record from first index of first question record 
    u_int64_t offset_with_multiple_records = 0;

    // Iterate through NoQuestions
    for (int i = 0; i < packet_info->dns_record_struct.NoQuestions; i++)
    {
        // Stores length of domain name
        uint16_t qname_length = 0;
            qname_length = name_length(((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records), packet_info, packet_raw, NULL, NULL);   //Name length returns value without last 0x00 byte!

        // printf("Delka question name je: %u\n", qname_length); //Debug

        packet_info->dns_non_header_struct.arr_questions[i].QNAME = (u_char *)malloc((qname_length+2) * sizeof(u_char)); //We have to malloc qname_length+2 bcs of last '\0' char (+1 to be sure :D)
            if (packet_info->dns_non_header_struct.arr_questions[i].QNAME == NULL)
            {
                fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
                fprintf(stderr, "Error, malloc returned NULL pointer in function parse_question_section()3\n");
                fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
                return NULL;
            }


        packet_info->dns_non_header_struct.arr_questions[i].QNAME[0] = '\0';

        load_domain_name(packet_info->dns_non_header_struct.arr_questions[i].QNAME, ((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records), qname_length+1, packet_raw,packet_info, true);    

        // printf("Obsah QNAME je: %s\n", packet_info->dns_non_header_struct.arr_questions->QNAME);    //Debug

        // Calculate QTYPE and QCLASS indexes
        u_char QTYPE_arr[3] ;
            QTYPE_arr[0] = *((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records + qname_length + 2);
            QTYPE_arr[1] = *((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records +qname_length + 3);
            QTYPE_arr[2] = '\0';
        u_char QCLASS_arr[3] ;
            QCLASS_arr[0] = *((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records + qname_length + 4);
            QCLASS_arr[1] = *((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records + qname_length + 5);
            QCLASS_arr[2] = '\0';

        // Store values into packet_info struct
        packet_info->dns_non_header_struct.arr_questions[i].QTYPE[0] = QTYPE_arr[0];
        packet_info->dns_non_header_struct.arr_questions[i].QTYPE[1] = QTYPE_arr[1];
        packet_info->dns_non_header_struct.arr_questions[i].QTYPE[2] = '\0';
        
        packet_info->dns_non_header_struct.arr_questions[i].QCLASS[0] = QCLASS_arr[0];
        packet_info->dns_non_header_struct.arr_questions[i].QCLASS[1] = QCLASS_arr[1];
        packet_info->dns_non_header_struct.arr_questions[i].QCLASS[2] = '\0';

        // printf ("QTYPE_arr je: %0x %0x\n", packet_info->dns_non_header_struct.arr_questions[i].QTYPE[0], packet_info->dns_non_header_struct.arr_questions[i].QTYPE[1]); //Debug
        packet_info->dns_non_header_struct.arr_questions[i].end_question_ptr = NULL;
        // printf ("QCLASS_arr je: %0x %0x\n", packet_info->dns_non_header_struct.arr_questions[i].QCLASS[0], packet_info->dns_non_header_struct.arr_questions[i].QCLASS[1]);  //Debug
        
        //Store end ptr of QUESTION SECTION of dns packet
        packet_info->dns_non_header_struct.arr_questions[i].end_question_ptr = ((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + offset_with_multiple_records + qname_length + 5);
        // printf ("QUESTION endptr je: %0x %0x\n", *(packet_info->dns_non_header_struct.arr_questions[i].end_question_ptr), *(packet_info->dns_non_header_struct.arr_questions[i].end_question_ptr+1));  //Debug

        offset_with_multiple_records += ((qname_length+1) + 2 + 2 + 1); //QNAME(w '\0') + QTYPE + QCLASS + beginning of next question selection
    }
    
    
    
    // Return pointer to address of last parsed question 
    return packet_info->dns_non_header_struct.arr_questions[(packet_info->dns_record_struct.NoQuestions)-1].end_question_ptr;
}

u_char* parse_resource_record_section (struct_packet_info* packet_info, u_char *packet_raw, u_int16_t NoRecords, resource_record_section *destination, RR_type type_of_record_enum ){
    if (packet_info == NULL || packet_raw == NULL || destination == NULL)
    {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
        fprintf(stderr, "Error, received NULL pointer in parse_question_section()\n");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
        return NULL;
    }

    // Offset_with_multiple_records defined offset of each question record from first index of first question record
    u_int64_t offset_with_multiple_records = 0;
    // Pointer to the last parsed part of packet
    u_char *end_of_last_record_type = NULL;
    bool name_set_by_pointer = false;

    // Get pointer to the last byte of previous record
    if (type_of_record_enum == Answer)
    {
        // printf("Actual record type is <Answer> \n");    //Debug
        if (packet_info->dns_record_struct.NoAnswers != 0)
        {
            end_of_last_record_type = packet_info->dns_non_header_struct.arr_questions[(packet_info->dns_record_struct.NoQuestions)-1].end_question_ptr;
        }
    }
    else if (type_of_record_enum == Authority)
    {
        // printf("Actual record type is <Authority> \n");    //Debug
        if (packet_info->dns_record_struct.NoAnswers != 0)
        {
            end_of_last_record_type = packet_info->dns_non_header_struct.arr_answers[(packet_info->dns_record_struct.NoAnswers)-1].end_record_ptr;
        }
        else if (packet_info->dns_record_struct.NoQuestions != 0)
        {
            end_of_last_record_type = packet_info->dns_non_header_struct.arr_questions[(packet_info->dns_record_struct.NoQuestions)-1].end_question_ptr;
        }
    }
    else if (type_of_record_enum == Additional)
    {
        // printf("Actual record type is <Additional> \n");    //Debug
        if (packet_info->dns_record_struct.NoAuthoritys != 0)
        {
            end_of_last_record_type = packet_info->dns_non_header_struct.arr_authorities[(packet_info->dns_record_struct.NoAuthoritys)-1].end_record_ptr;
        }
        else if (packet_info->dns_record_struct.NoAnswers != 0)
        {
            end_of_last_record_type = packet_info->dns_non_header_struct.arr_answers[(packet_info->dns_record_struct.NoAnswers)-1].end_record_ptr;
        }
        else if (packet_info->dns_record_struct.NoQuestions != 0)
        {
            end_of_last_record_type = packet_info->dns_non_header_struct.arr_questions[(packet_info->dns_record_struct.NoQuestions)-1].end_question_ptr;
        }
    }
    
    // printf ("Pointer end_of_last_record_type ukazuje na: last:%0x \n", *(end_of_last_record_type)); //Debug

    // Iterate through NoRecords
    for (int i = 0; i < NoRecords; i++)
    {   
        name_set_by_pointer = false;
        // CHECK IT variable 'offset' is set to 0 and used only once, it is only 0 not modified at any time
        u_int16_t name_offset_in_packet = 0;

        // printf("Offset_with_multiple_records: %lu\n", offset_with_multiple_records);

        // name_length() function sets $name_set_by_pointer and $name_offset_in_packet
        // if $name_set_by_pointer is true, then function returns value constant value of 1
        // real length of the NAME we can get with $name_offset_in_packet and calling it again with right offset
        uint16_t RR_name_length = name_length((end_of_last_record_type +1 + offset_with_multiple_records), packet_info, packet_raw, &name_set_by_pointer, &name_offset_in_packet);   //Name length returns value without last 0x00 byte!

        destination[i].name_given_by_pointer = name_set_by_pointer;

        if (name_set_by_pointer == true)    
        {
            // printf ("Hodnota NAME je dana ukazatelem pred load_domain_name \n"); // Debug
            load_domain_name(destination[i].NAME, (packet_info->packet_lengths.dns_header_ptr + name_offset_in_packet), 0, packet_raw,packet_info, true);
            RR_name_length = NAME_GIVEN_BY_PTR;
        }
        else
        {
            // printf ("Hodnota NAME je dana hodnotou pred load_domain_name\n"); // Debug
            load_domain_name(destination[i].NAME, (end_of_last_record_type + offset_with_multiple_records), RR_name_length, packet_raw,packet_info, true); 
            RR_name_length++; // Check!
        }

        // Calculate TYPE indexes
        u_char TYPE_arr[3] ;
            TYPE_arr[0] = '0';
            TYPE_arr[1] = '0';
            TYPE_arr[2] = '\0';

            TYPE_arr[0] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+1);
            TYPE_arr[1] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+2);
            TYPE_arr[2] = '\0';
        // Calculate CLASS indexes
        u_char CLASS_arr[3] ;
            CLASS_arr[0] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+3);
            CLASS_arr[1] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+4);
            CLASS_arr[2] = '\0';
        // Calculate TTL indexes
        u_char TTL_arr[5] ;
            TTL_arr[0] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+5);
            TTL_arr[1] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+6);
            TTL_arr[2] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+7);
            TTL_arr[3] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+8);
            TTL_arr[4] = '\0';
        // Calculate RDLENGTH indexes
        u_char RDLENGTH[3] ;
            RDLENGTH[0] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+9);
            RDLENGTH[1] = *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+10);
            RDLENGTH[2] = '\0';
        // Store values into packet_info struct
        destination[i].TYPE[0] = TYPE_arr[0];
        destination[i].TYPE[1] = TYPE_arr[1];
        destination[i].TYPE[2] = '\0';
        
        destination[i].CLASS[0] = CLASS_arr[0];
        destination[i].CLASS[1] = CLASS_arr[1];
        destination[i].CLASS[2] = '\0';

        // printf("RDLENGTH v bytech jeRDLENGTH[0]: %0x, RDLENGTH[1] %0x\n", RDLENGTH[0], RDLENGTH[1]);
        destination[i].RDLENGTH = u_CharArr_2_u_Int16(RDLENGTH);

        destination[i].TTL = u_CharArr_4_Int32(TTL_arr);

        // printf ("delka RDATA je:%u\n", destination[i].RDLENGTH); //Debug

        destination[i].RDATA = (u_char *)malloc((MAX_DOMAIN_NAME) * sizeof(u_char));
            if (destination[i].RDATA == NULL)
            {
                fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
                fprintf(stderr, "malloc error, destination[i].RDATA is NULL\n");
                fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
                return NULL;
            }
        destination[i].RDATA[0] = '\0';    

        // printf("RDATA ctu z hodnot: %0x %0x \n", *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+11), *(end_of_last_record_type+RR_name_length+offset_with_multiple_records+12));
        
        // A record
        if (destination[i].TYPE[0] == 0x00 && destination[i].TYPE[1] == 0x01 )
        {
            u_char ipv4_hexa[4];
    
            for (int a = 0; a < 4; a++) {
                ipv4_hexa[a] = *(end_of_last_record_type + RR_name_length + offset_with_multiple_records + 11 + a);
            }
            
            struct in_addr ip_addr;
            memcpy(&ip_addr.s_addr, ipv4_hexa, 4);
            
            // Convert the IPv4 address to a string
            
            strcpy((char *) destination[i].RDATA, inet_ntoa(ip_addr));
            write_to_file(destination[i].NAME, inet_ntoa(ip_addr));
        }
        // AAAA record
        else if (TYPE_arr[0] == 0x00 && TYPE_arr[1] == 0x1c)
        {
            unsigned char ipv6_address[16];  // IPv6 adresa je vždy 16 bajtů

            // Zkopíruj IPv6 adresu z DNS odpovědi (předpokládáme, že IPv6 adresa je ve formátu 16 bajtů v RDATA)
            memcpy(ipv6_address, end_of_last_record_type + RR_name_length + offset_with_multiple_records + 11 , 16);

            // Převod IPv6 adresy na řetězec
            char ipv6_str[INET6_ADDRSTRLEN];  // Pro IPv6 string potřebujeme dostatečnou velikost (46 znaků)
            convert_ipv6_to_str(ipv6_address, ipv6_str);

            // Ulož IPv6 adresu do RDATA jako řetězec
            strcpy((char *)destination[i].RDATA, ipv6_str);
            write_to_file(destination[i].NAME, ipv6_str);
        }
        // NS record
        else if (TYPE_arr[0] == 0x00 && TYPE_arr[1] == 0x02)
        {
            load_domain_name(destination[i].RDATA, end_of_last_record_type+RR_name_length+offset_with_multiple_records+11, 0 , packet_raw, packet_info, true );
        }
        // MX record
        else if (TYPE_arr[0] == 0x00 && TYPE_arr[1] == 0x0f)
        {
            parse_MX(packet_info, &destination[i].MX_record, (end_of_last_record_type+RR_name_length+offset_with_multiple_records+11), packet_raw);
        }
        // CNAME record
        else if (TYPE_arr[0] == 0x00 && TYPE_arr[1] == 0x05)
        {
            load_domain_name(destination[i].RDATA, end_of_last_record_type+RR_name_length+offset_with_multiple_records+11, 0 , packet_raw, packet_info, true );
        }
        // SOA record, KNOWN BUG  when class == HTTP, sometimes it jump into SOA record...
        else if (TYPE_arr[0] == 0x00 && TYPE_arr[1] == 0x06)
        {
            parse_SOA(packet_info, &destination[i].SOA_record ,(end_of_last_record_type+RR_name_length+offset_with_multiple_records+11), packet_raw );
        }
        // SRV record
        else if (TYPE_arr[0] == 0x00 && TYPE_arr[1] == 0x21)
        {
            parse_SRV(packet_info, &destination[i].SRV_record ,(end_of_last_record_type+RR_name_length+offset_with_multiple_records+11), packet_raw );
        }
        else
        {

        }

        
        //Store end ptr of QUESTION SECTION of dns packet
        packet_info->dns_non_header_struct.arr_questions->end_question_ptr = ((packet_info->packet_lengths.dns_header_ptr + (int) DNS_HEADER_LENGTH) + RR_name_length + 5 + offset_with_multiple_records);

        destination[i].end_record_ptr = (end_of_last_record_type + offset_with_multiple_records + RR_name_length + 10 + destination[i].RDLENGTH);

        offset_with_multiple_records += ((RR_name_length) + 2 + 2 + 4 + 2 + destination[i].RDLENGTH); //NAME(w ) + TYPE + CLASS + TTL + RDLENGTH + len(RDATA)
        // printf("offset_with_multiple_records je: %lu\n",offset_with_multiple_records);
        // printf("-----------------------------------------------------------\n");
    }

    return NULL;
}

void fill_non_header_dns_parts(struct_packet_info* packet_info, u_char *packet_raw, bool verbose){
    if (packet_info == NULL || packet_raw == NULL)
    {
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        fprintf (stderr, "Error, function fill_non_header_dns_parts() received NULL ptr\n");
        fprintf(stderr, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
        return;
    }
    
    dns_parser_setup(packet_info, packet_raw);
    global_verbose = verbose;
    if (packet_info->dns_record_struct.NoQuestions != 0)
    {
        parse_question_section(packet_info, packet_raw);
        // printf("//////////////////////////////////////////////////////////\n");
    }
    

    if (packet_info->dns_record_struct.NoAnswers != 0)
    {
        // printf("Answer\n");
        // printf("Number of Answer record is: %u\n", packet_info->dns_record_struct.NoAnswers);
        parse_resource_record_section(packet_info, packet_raw, packet_info->dns_record_struct.NoAnswers, packet_info->dns_non_header_struct.arr_answers, Answer);
        // printf("//////////////////////////////////////////////////////////\n");
    }
        
    if (packet_info->dns_record_struct.NoAuthoritys != 0)
    {
        // printf("Authority\n");
        // printf("Number of Authority record is: %u\n", packet_info->dns_record_struct.NoAuthoritys);
        parse_resource_record_section(packet_info, packet_raw, packet_info->dns_record_struct.NoAuthoritys, packet_info->dns_non_header_struct.arr_authorities, Authority);
        // printf("//////////////////////////////////////////////////////////\n");
    }
    
    if (packet_info->dns_record_struct.NoAdditions != 0)
    {
        // printf("Additional\n");
        // printf("Number of Additional record is: %u\n", packet_info->dns_record_struct.NoAdditions);
        parse_resource_record_section(packet_info, packet_raw, packet_info->dns_record_struct.NoAdditions, packet_info->dns_non_header_struct.arr_additional, Additional);
        // printf("//////////////////////////////////////////////////////////\n");
    }
    

    

    
    // printf ("Answer cast zacina: %0x %0x %0x %0x \n", *question_end, *(question_end+1), *(question_end+2), *(question_end+3)); //<Last Question part>, <1st A>, <2nd A>, <3rd A>
    if (global_verbose == true)
    {
        print_verbose_dns_data(packet_info, verbose);
    }

    dns_parser_clean();
}

/**
 * End of file dns_parser.c
 */
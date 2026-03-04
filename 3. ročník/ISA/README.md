Author: Jiri Kotek
xlogin: xkotek09
School: BUT FIT
Date: 15. 11. 2024

Project name: DNS monitor


Project detail: The tool will process DNS protocol messages and display extracted information. Additionally, it will be capable of identifying domain names that appear in DNS messages. The third functionality is translating domain names to IPv4/IPv6 addresses.The program provides three possible outputs:
    Standard output with information about DNS messages.
    (Optional) A file containing the observed domain names.
    (Optional) A file with mappings of domain names to IP addresses.


Project run: ./dns-monitor (-i <interface> | -p <pcapfile>) [-v] [-d <domainsfile>] [-t <translationsfile>]
Parameters:

-i <interface> - the name of the network interface the program will listen on, or
-p <pcapfile> - the name of the PCAP file to be processed by the program;
-v - "verbose" mode: complete details of DNS messages will be displayed;
-d <domainsfile> - the name of the file to store domain names;
-t <translationsfile> - the name of the file to store mappings of domain names to IP addresses.

Project files: 
    /src
        | - argument_parser.c
        | - argument_parser.h
        | - dns_parse.c
        | - dns_parse.h
        | - file_printer.c
        | - file_printer.h
        | - libcap_engine.c
        | - libcap_engone.h
        | - main.c
        | - main.h
    -Makefile
    -README.md
    -manual.pdf

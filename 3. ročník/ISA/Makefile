EXEC = dns-monitor
CC = clang
CFLAGS = -Wall -std=gnu99 -MMD
LDLIBS = -lpcap

SRC_DIR = src
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/argument_parser.c $(SRC_DIR)/dns_parser.c $(SRC_DIR)/file_printer.c $(SRC_DIR)/libcap_engine.c
OBJS = main.o argument_parser.o dns_parser.o file_printer.o libcap_engine.o
DEPS = main.d argument_parser.d dns_parser.d file_printer.d libcap_engine.d

.PHONY: clean all clean_obj

all: $(EXEC)
	$(MAKE) clean_obj  # Clean up .o and .d files after building the executable

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDLIBS)

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o main.o

argument_parser.o: $(SRC_DIR)/argument_parser.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/argument_parser.c -o argument_parser.o

dns_parser.o: $(SRC_DIR)/dns_parser.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/dns_parser.c -o dns_parser.o

file_printer.o: $(SRC_DIR)/file_printer.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/file_printer.c -o file_printer.o

libcap_engine.o: $(SRC_DIR)/libcap_engine.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/libcap_engine.c -o libcap_engine.o

-include $(DEPS)

clean:
	rm -f $(EXEC) $(OBJS) $(DEPS)

clean_obj:
	rm -f $(OBJS) $(DEPS)

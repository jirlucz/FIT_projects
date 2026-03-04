# Makefile for scanner_test.c

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = scanner_test.c
OBJ = $(SRC:.c=.o)
EXEC = scanner

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean

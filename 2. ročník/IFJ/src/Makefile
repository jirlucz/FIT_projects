CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm
FILES=parser.c expr.c scanner.c generator.c stack.c stackChar.c symtable.c
BINARIES=test main

ifdef DEBUG
CFLAGS += -DDEBUG
endif

.PHONY: test clean

main: FILES+=main.c
main: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

test: FILES+=compilerTest.c
test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	rm -f $(BINARIES)

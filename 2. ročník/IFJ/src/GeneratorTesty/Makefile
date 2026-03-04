CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror
COMPILE=$(CC) $(CFLAGS)

ifdef DEBUG
CFLAGS += -DDEBUG
endif

.PHONY: clean all run

all: TEST


.SILENT:

TEST: generator.o generatorTest.o
	$(COMPILE) -o $@ $^ -lm
	echo "Linking complete."

generatorTest.o: generatorTest.c ../generator.c ../generator.h ../scanner.h ../scanner.c
	$(COMPILE) -c -o $@ generatorTest.c
	echo "Compilation of generatorTest.o complete."

generator.o: ../generator.c ../generator.h ../fluid_string.h ../fluid_string.c
	$(COMPILE) -c -o $@ ../generator.c
	echo "Compilation of generator.o complete."

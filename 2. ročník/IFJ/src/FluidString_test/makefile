CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = fluid_cmp_test.c
TARGET = test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

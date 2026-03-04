EXEC = ipk24chat-client
CC = gcc
CFLAGS = -Wall -std=gnu99 -MMD
LDLIBS = -lm -lnsl

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
DEPS = $(SRCS:.c=.d)

.PHONY: clean all

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEPS)

clean:
	rm -f $(EXEC) $(OBJS) $(DEPS)

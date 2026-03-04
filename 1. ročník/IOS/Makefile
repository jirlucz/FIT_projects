EXEC = proj2
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS = -lm -pthread

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
DEPS = $(SRCS:.c=.d)

.PHONY: clean all

all: $(EXEC)

$(EXEC): $(OBJS)

-include $(DEPS)

clean:
	rm -f $(EXEC) $(OBJS) $(DEPS)

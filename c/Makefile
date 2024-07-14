CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic

TARGET = brainfuck
SRCS = brainfuck.c interpreter.c hashmap.c stack.c
HEADERS = hashmap.h interpreter.h stack.h

OBJS = $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

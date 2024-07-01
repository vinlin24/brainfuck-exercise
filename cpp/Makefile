CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -Wpedantic

TARGET = brainfuck
SRCS = brainfuck.cc interpreter.cc
HEADERS = interpreter.h

OBJS = $(SRCS:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

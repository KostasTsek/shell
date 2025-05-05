# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Source files
SRCS = hy345sh.c terminalHandle.c

# Header files
HEADERS = terminalHandle.h

TARGET = hy345sh

# Default target
all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean target to remove the executable and any object files
clean:
	rm -f $(TARGET) *.o

# Phony targets
.PHONY: all clean


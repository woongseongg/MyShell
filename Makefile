# Compiler settings
CC = gcc
CFLAGS = -Iinclude -Wall

# Target file
TARGET = bin/myshell

# Source files, object files, and target settings
SRCS = src/myshell.c src/builtin_commands.c 
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to create the myshell executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# Compile individual object files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Simple Makefile for main.c and chunk.c

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Sources
SRCS = main.c chunk.c memory.c debug.c value.c vm.c scanner.c compiler.c

# Directories
BUILD_DIR = build

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
TARGET = $(BUILD_DIR)/clox

# Default target
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

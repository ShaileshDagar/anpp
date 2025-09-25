# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Directories
SRC_DIR = src
BIN_DIR = bin

# Find all .c files in src/
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Corresponding executables in bin/
TARGETS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRC_FILES))

# Default target: build all executables
all: $(TARGETS)

# Rule to build each executable
$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean all executables
clean:
	rm -rf $(BIN_DIR)/*
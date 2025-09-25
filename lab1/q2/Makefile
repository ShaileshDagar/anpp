# Compiler and default flags
CC = gcc
CFLAGS = -Wall -g

# Directories
SRC_DIR = src
BIN_DIR = bin

# Find all .c files in src/
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Corresponding executables in bin/
TARGETS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRC_FILES))

# Extra flags for specific source files
# Format: SRC_FILE := EXTRA_FLAGS
EXTRA_FLAGS_server.c = -lm
# Add more files here if needed:
# EXTRA_FLAGS_other.c = -lfoo -lbar

# Default target: build all executables
all: $(TARGETS)

# Rule to compile each executable
$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(EXTRA_FLAGS_$(notdir $<))

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean all executables
clean:
	rm -rf $(BIN_DIR)/*
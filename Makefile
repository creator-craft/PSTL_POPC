# Project configuration
SRC_DIR       := src
INC_DIR       := include
BUILD_DIR     := build
TARGET        := $(BUILD_DIR)/app
IMAGES_TARGET := $(BUILD_DIR)/read_images

CC          := gcc
SDL_CFLAGS  := $(shell sdl2-config --cflags)
SDL_LIBS    := -lSDL2

# Source and object files
IMAGES_SRCS := src/POP_utils/read_images.c
SRCS        := $(wildcard src/POP_SDL/*.c)
OBJS        := $(SRCS:src/*/%.c=src/*/%.o)

# Common flags
CFLAGS_COMMON := -std=c11 -Wall -Wextra -I$(INC_DIR) $(SDL_CFLAGS)
LDFLAGS       := $(SDL_LIBS) -I$(INC_DIR)

# Release / Debug flags
CFLAGS_RELEASE := -Ofast -DNDEBUG
CFLAGS_DEBUG   := -O0 -g -DDEBUG

# Default target
.PHONY: all
all: release

# Release build
.PHONY: release
release: CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_RELEASE)
release: $(TARGET)

# Debug build
.PHONY: debug
debug: CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
debug: $(TARGET)

# Debug build and run
.PHONY: run
run: debug
	./$(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Read images
images: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(IMAGES_SRCS) -o $(IMAGES_TARGET) $(LDFLAGS) -lSDL2_image

# Clean
.PHONY: clean
clean:
	rm -f ./$(BUILD_DIR)/*

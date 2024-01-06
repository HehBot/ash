CC := gcc
LD := gcc
TARGET_NAME := abash

BUILD_DIR := build
BIN_DIR := bin
SRC_DIR := src

TARGET := $(BIN_DIR)/$(TARGET_NAME)

MAIN := abash

SRCS := $(shell find $(SRC_DIR) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := -I$(SRC_DIR)

CC_FLAGS := -g -Wall -Wpedantic -Werror -MMD -MP $(INC_FLAGS)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $^

$(BUILD_DIR)/%.c.o:%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CC_FLAGS) -o $@ $<

.PHONY: all clean test

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

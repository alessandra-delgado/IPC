.PHONY: all run clean

# ! Gotta change this makefile later
CC := g++
EXE := execute.exe
BUILD_DIR := ./build
SRC_DIR := src
EXE_DIR := $(BUILD_DIR)/$(EXE)
MKDIR := mkdir -p

# Flags de compilação
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic

# LIBS := sem libs ainda...

SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

all: comp
run: $(EXE_DIR)
	./$< $(ARGS)

# COMPILE =====================
comp: $(EXE_DIR)

$(EXE_DIR): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o : %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CXXFLAGS) -c $< -o $@

# =============================
clean:
	rm -rf $(BUILD_DIR)
	@echo "Ficheiros removidos."
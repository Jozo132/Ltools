# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Iinclude
LDFLAGS := -Llib
LDLIBS := # Add any libraries here

# Directories
SRC_DIR := src
INCLUDE_DIR := include
RESOURCES_DIR := resources
LIB_DIR := lib
BUILD_DIR := build

# Build types
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2

# Files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
TARGET := main

# Default target
all: release

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)
	./$(TARGET)

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)
	./$(TARGET)

# Link target
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$@ $(LDFLAGS) $(LDLIBS)
	move $(BUILD_DIR)\$@.exe .

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all debug release clean

# Compiler and flags
CXX := g++
# CXXFLAGS := -std=c++11 -Wall -Iinclude -march=native -mpclmul -maes
CXXFLAGS := -std=c++11 -Iinclude -lfreetype -march=native -mpclmul -maes
LDFLAGS := -Llib -lfreetype
LDLIBS := # Add any libraries here

# Directories
SRC_DIR := src
INCLUDE_DIR := include
RESOURCES_DIR := resources
LIB_DIR := lib
BUILD_DIR := build
WIN_BUILD_DIR := C:/win-build/include

# Build types
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -Os

CXXFLAGS += -I$(WIN_BUILD_DIR)/freetype2

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
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

clean_main:
	@if exist $(BUILD_DIR)\$(TARGET).o del $(BUILD_DIR)\$(TARGET).o

.PHONY: all debug release clean

# Compiler and flags
CXX := g++
# CXXFLAGS := -std=c++11 -Wall -Iinclude -march=native -mpclmul -maes
CXXFLAGS := -MD -std=c++11 -Iinclude -Ilib -lfreetype -march=native -mpclmul -maes -lpsapi -lz
LDFLAGS := -Llib -lfreetype -lpsapi -lz
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
INPUT := main
TARGET := zpl2png
SAMPLE := zpl_sample.zpl

FINAL_DIR := C:/Ltools
FINAL_BIN := "$(FINAL_DIR)/$(TARGET).exe"

# Default target
all: release


# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(INPUT)
	./$(TARGET) $(SAMPLE)

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(INPUT)
	./$(TARGET) $(SAMPLE)

# Link target
$(INPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$@ $(LDFLAGS) $(LDLIBS)
	@echo off
	@move $(BUILD_DIR)\$(INPUT).exe ./$(TARGET).exe
	@echo on

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@if exist $(BUILD_DIR)\$(INPUT).o del $(BUILD_DIR)\$(INPUT).o
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Move executable to final directory and run ./addToContextMenu.reg
install: CXXFLAGS += $(RELEASE_FLAGS)
install: $(INPUT)
	@if not exist "$(FINAL_DIR)" mkdir "$(FINAL_DIR)"
	@echo Moving executable to $(FINAL_BIN)
	@copy "./$(TARGET).exe" $(FINAL_BIN)
	@echo Adding to context menu...
	@powershell.exe -Command "Start-Process regedit.exe -ArgumentList '/s ./addToContextMenu.reg' -Verb RunAs"

# Clean
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

clean_main:
	@if exist $(BUILD_DIR)\$(INPUT).o del $(BUILD_DIR)\$(INPUT).o

.PHONY: all debug release clean

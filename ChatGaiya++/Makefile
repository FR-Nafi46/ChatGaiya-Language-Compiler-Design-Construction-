CXX ?= g++
FLEX ?= flex
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
BUILD_DIR := .build
SOURCES := main.cpp parser.cpp compiler.cpp error_reporting.cpp symbol_table.cpp semantic_analysis.cpp code_generation.cpp

.PHONY: all clean test
all: chatgaiya

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/lexer.generated.cpp: lexer.l tokens.hpp error_reporting.hpp | $(BUILD_DIR)
	$(FLEX) -o $@ lexer.l

chatgaiya: $(SOURCES) compiler.hpp error_reporting.hpp symbol_table.hpp semantic_analysis.hpp code_generation.hpp parser.hpp tokens.hpp $(BUILD_DIR)/lexer.generated.cpp
	$(CXX) $(CXXFLAGS) -I. -o $@ $(SOURCES) $(BUILD_DIR)/lexer.generated.cpp

clean:
	rm -rf $(BUILD_DIR) chatgaiya chatgaiya.exe

test: chatgaiya
	python3 tests/run_tests.py

# L# Compiler Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g
FLEX = flex
BISON = bison
ANTLR = antlr4

# Directories
SRC_DIR = .
BUILD_DIR = build
BIN_DIR = bin

# Source files
LEXER_SRC = lexer.y
PARSER_SRC = parser.y
AST_SRC = ast.c
SEMANTIC_SRC = semantic.c
CODEGEN_SRC = codegen.c
MAIN_SRC = main.c

# Generated files
LEXER_GEN = $(BUILD_DIR)/lex.yy.c
PARSER_GEN = $(BUILD_DIR)/parser.tab.c
PARSER_HEADER = $(BUILD_DIR)/parser.tab.h

# Object files
OBJS = $(BUILD_DIR)/lex.yy.o \
       $(BUILD_DIR)/parser.tab.o \
       $(BUILD_DIR)/ast.o \
       $(BUILD_DIR)/semantic.o \
       $(BUILD_DIR)/codegen.o \
       $(BUILD_DIR)/main.o

# Output
TARGET = $(BIN_DIR)/lsc

# ANTLR files
ANTLR_GRAMMAR = LSharp.g4
ANTLR_OUTPUT = $(BUILD_DIR)/antlr

.PHONY: all clean flex-bison antlr test install

all: $(TARGET)

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Generate lexer
$(LEXER_GEN): $(LEXER_SRC) | $(BUILD_DIR)
	$(FLEX) -o $(LEXER_GEN) $(LEXER_SRC)

# Generate parser
$(PARSER_GEN) $(PARSER_HEADER): $(PARSER_SRC) | $(BUILD_DIR)
	$(BISON) -d -o $(PARSER_GEN) $(PARSER_SRC)

# Compile lexer
$(BUILD_DIR)/lex.yy.o: $(LEXER_GEN) $(PARSER_HEADER)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c $(LEXER_GEN) -o $@

# Compile parser
$(BUILD_DIR)/parser.tab.o: $(PARSER_GEN) $(PARSER_HEADER)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c $(PARSER_GEN) -o $@

# Compile AST
$(BUILD_DIR)/ast.o: $(AST_SRC) ast.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c $(AST_SRC) -o $@

# Compile semantic analyzer
$(BUILD_DIR)/semantic.o: $(SEMANTIC_SRC) semantic.h ast.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c $(SEMANTIC_SRC) -o $@

# Compile code generator
$(BUILD_DIR)/codegen.o: $(CODEGEN_SRC) codegen.h ast.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c $(CODEGEN_SRC) -o $@

# Compile main
$(BUILD_DIR)/main.o: $(MAIN_SRC) ast.h semantic.h codegen.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BUILD_DIR) -c $(MAIN_SRC) -o $@

# Link
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lfl

# Flex/Bison build
flex-bison: $(TARGET)

# ANTLR build
antlr: $(ANTLR_GRAMMAR) | $(BUILD_DIR)
	mkdir -p $(ANTLR_OUTPUT)
	$(ANTLR) -Dlanguage=Cpp -o $(ANTLR_OUTPUT) $(ANTLR_GRAMMAR)
	@echo "ANTLR parser generated in $(ANTLR_OUTPUT)"

# Test
test: $(TARGET)
	@echo "Running compiler tests..."
	./$(TARGET) ../examples/hello-world.ls
	./$(TARGET) ../examples/ui-component.ls
	@echo "Tests completed"

# Install
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/lsc
	@echo "L# compiler installed to /usr/local/bin/lsc"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Build artifacts cleaned"

# Help
help:
	@echo "L# Compiler Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all         - Build the compiler (default)"
	@echo "  flex-bison  - Build using Flex/Bison"
	@echo "  antlr       - Generate ANTLR parser"
	@echo "  test        - Run compiler tests"
	@echo "  install     - Install compiler to system"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make              # Build compiler"
	@echo "  make test         # Run tests"
	@echo "  make install      # Install system-wide"

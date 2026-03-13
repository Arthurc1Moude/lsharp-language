/* L# Code Generator Header */
/* Bytecode generation for Geneia platform */

#ifndef LSHARP_CODEGEN_H
#define LSHARP_CODEGEN_H

#include "ast.h"
#include <stdio.h>

/* ============================================================================
 * BYTECODE OPCODES
 * ============================================================================
 */

typedef enum {
    OP_NOP = 0x00,
    OP_LOAD_CONST = 0x01,
    OP_LOAD_VAR = 0x02,
    OP_STORE_VAR = 0x03,
    OP_ADD = 0x10,
    OP_SUB = 0x11,
    OP_MUL = 0x12,
    OP_DIV = 0x13,
    OP_MOD = 0x14,
    OP_POW = 0x15,
    OP_NEG = 0x16,
    OP_EQ = 0x20,
    OP_NE = 0x21,
    OP_LT = 0x22,
    OP_LE = 0x23,
    OP_GT = 0x24,
    OP_GE = 0x25,
    OP_AND = 0x30,
    OP_OR = 0x31,
    OP_NOT = 0x32,
    OP_JUMP = 0x40,
    OP_JUMP_IF_FALSE = 0x41,
    OP_JUMP_IF_TRUE = 0x42,
    OP_CALL = 0x50,
    OP_RETURN = 0x51,
    OP_PUSH = 0x60,
    OP_POP = 0x61,
    OP_DUP = 0x62,
    OP_SWAP = 0x63,
    OP_HALT = 0xFF
} Opcode;

/* ============================================================================
 * CODE GENERATION STRUCTURES
 * ============================================================================
 */

typedef struct BytecodeChunk {
    unsigned char *code;
    int capacity;
    int count;
    int *lines;
} BytecodeChunk;

typedef struct CodeGenerator {
    BytecodeChunk *chunk;
    FILE *output;
    int error_count;
} CodeGenerator;

/* ============================================================================
 * CODE GENERATION FUNCTIONS
 * ============================================================================
 */

/* Initialize code generator */
CodeGenerator* create_code_generator(FILE *output);
void free_code_generator(CodeGenerator *gen);

/* Bytecode chunk operations */
BytecodeChunk* create_chunk();
void free_chunk(BytecodeChunk *chunk);
void write_byte(BytecodeChunk *chunk, unsigned char byte, int line);
void write_opcode(BytecodeChunk *chunk, Opcode op, int line);

/* Code generation from AST */
void generate_code(CodeGenerator *gen, ASTNode *node);
void generate_expression(CodeGenerator *gen, ASTNode *node);
void generate_statement(CodeGenerator *gen, ASTNode *node);
void generate_function(CodeGenerator *gen, ASTNode *node);

/* Constant pool management */
int add_constant(BytecodeChunk *chunk, void *value);
void* get_constant(BytecodeChunk *chunk, int index);

/* Label and jump management */
int emit_jump(BytecodeChunk *chunk, Opcode op);
void patch_jump(BytecodeChunk *chunk, int offset);

/* Output bytecode file */
void write_bytecode_file(CodeGenerator *gen, const char *filename);

/* Disassembler */
void disassemble_chunk(BytecodeChunk *chunk, const char *name);
int disassemble_instruction(BytecodeChunk *chunk, int offset);

#endif /* LSHARP_CODEGEN_H */

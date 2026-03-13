/* L# Code Generator Implementation */
/* Bytecode generation for Geneia platform */

#include "codegen.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * BYTECODE CHUNK IMPLEMENTATION
 * ============================================================================
 */

BytecodeChunk* create_chunk() {
    BytecodeChunk *chunk = (BytecodeChunk*)malloc(sizeof(BytecodeChunk));
    chunk->capacity = 8;
    chunk->count = 0;
    chunk->code = (unsigned char*)malloc(chunk->capacity);
    chunk->lines = (int*)malloc(chunk->capacity * sizeof(int));
    return chunk;
}

void free_chunk(BytecodeChunk *chunk) {
    if (!chunk) return;
    free(chunk->code);
    free(chunk->lines);
    free(chunk);
}

void write_byte(BytecodeChunk *chunk, unsigned char byte, int line) {
    if (chunk->count >= chunk->capacity) {
        chunk->capacity *= 2;
        chunk->code = (unsigned char*)realloc(chunk->code, chunk->capacity);
        chunk->lines = (int*)realloc(chunk->lines, chunk->capacity * sizeof(int));
    }
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

void write_opcode(BytecodeChunk *chunk, Opcode op, int line) {
    write_byte(chunk, (unsigned char)op, line);
}

/* ============================================================================
 * CODE GENERATOR IMPLEMENTATION
 * ============================================================================
 */

CodeGenerator* create_code_generator(FILE *output) {
    CodeGenerator *gen = (CodeGenerator*)malloc(sizeof(CodeGenerator));
    gen->chunk = create_chunk();
    gen->output = output;
    gen->error_count = 0;
    return gen;
}

void free_code_generator(CodeGenerator *gen) {
    if (!gen) return;
    free_chunk(gen->chunk);
    free(gen);
}

/* ============================================================================
 * CODE GENERATION FROM AST
 * ============================================================================
 */

void generate_expression(CodeGenerator *gen, ASTNode *node) {
    if (!gen || !node) return;
    
    switch (node->type) {
        case AST_INT_LITERAL:
            write_opcode(gen->chunk, OP_LOAD_CONST, node->line);
            /* Write constant index */
            break;
            
        case AST_BINARY_EXPR:
            /* Generate left operand */
            generate_expression(gen, node->data.binary_expr.left);
            /* Generate right operand */
            generate_expression(gen, node->data.binary_expr.right);
            
            /* Generate operator */
            const char *op = node->data.binary_expr.operator;
            if (strcmp(op, "+") == 0) {
                write_opcode(gen->chunk, OP_ADD, node->line);
            } else if (strcmp(op, "-") == 0) {
                write_opcode(gen->chunk, OP_SUB, node->line);
            } else if (strcmp(op, "*") == 0) {
                write_opcode(gen->chunk, OP_MUL, node->line);
            } else if (strcmp(op, "/") == 0) {
                write_opcode(gen->chunk, OP_DIV, node->line);
            }
            break;
            
        case AST_UNARY_EXPR:
            generate_expression(gen, node->data.unary_expr.operand);
            if (strcmp(node->data.unary_expr.operator, "-") == 0) {
                write_opcode(gen->chunk, OP_NEG, node->line);
            }
            break;
            
        case AST_IDENTIFIER:
            write_opcode(gen->chunk, OP_LOAD_VAR, node->line);
            /* Write variable index */
            break;
            
        default:
            break;
    }
}

void generate_statement(CodeGenerator *gen, ASTNode *node) {
    if (!gen || !node) return;
    
    switch (node->type) {
        case AST_VAR_DECL:
            if (node->data.var_decl.initializer) {
                generate_expression(gen, node->data.var_decl.initializer);
                write_opcode(gen->chunk, OP_STORE_VAR, node->line);
            }
            break;
            
        case AST_EXPR_STMT:
            generate_expression(gen, node->data.expr_stmt.expression);
            write_opcode(gen->chunk, OP_POP, node->line);
            break;
            
        case AST_IF_STMT: {
            /* Generate condition */
            generate_expression(gen, node->data.if_stmt.condition);
            
            /* Jump if false */
            int else_jump = emit_jump(gen->chunk, OP_JUMP_IF_FALSE);
            write_opcode(gen->chunk, OP_POP, node->line);
            
            /* Then branch */
            generate_statement(gen, node->data.if_stmt.then_branch);
            
            int end_jump = emit_jump(gen->chunk, OP_JUMP);
            patch_jump(gen->chunk, else_jump);
            write_opcode(gen->chunk, OP_POP, node->line);
            
            /* Else branch */
            if (node->data.if_stmt.else_branch) {
                generate_statement(gen, node->data.if_stmt.else_branch);
            }
            
            patch_jump(gen->chunk, end_jump);
            break;
        }
            
        case AST_RETURN_STMT:
            if (node->data.return_stmt.value) {
                generate_expression(gen, node->data.return_stmt.value);
            }
            write_opcode(gen->chunk, OP_RETURN, node->line);
            break;
            
        default:
            break;
    }
}

void generate_code(CodeGenerator *gen, ASTNode *node) {
    if (!gen || !node) return;
    generate_statement(gen, node);
}

/* ============================================================================
 * JUMP MANAGEMENT
 * ============================================================================
 */

int emit_jump(BytecodeChunk *chunk, Opcode op) {
    write_opcode(chunk, op, 0);
    write_byte(chunk, 0xFF, 0);
    write_byte(chunk, 0xFF, 0);
    return chunk->count - 2;
}

void patch_jump(BytecodeChunk *chunk, int offset) {
    int jump = chunk->count - offset - 2;
    chunk->code[offset] = (jump >> 8) & 0xFF;
    chunk->code[offset + 1] = jump & 0xFF;
}

/* ============================================================================
 * DISASSEMBLER
 * ============================================================================
 */

void disassemble_chunk(BytecodeChunk *chunk, const char *name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}

int disassemble_instruction(BytecodeChunk *chunk, int offset) {
    printf("%04d ", offset);
    
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }
    
    unsigned char instruction = chunk->code[offset];
    switch (instruction) {
        case OP_NOP:
            printf("OP_NOP\n");
            return offset + 1;
        case OP_LOAD_CONST:
            printf("OP_LOAD_CONST\n");
            return offset + 1;
        case OP_ADD:
            printf("OP_ADD\n");
            return offset + 1;
        case OP_SUB:
            printf("OP_SUB\n");
            return offset + 1;
        case OP_MUL:
            printf("OP_MUL\n");
            return offset + 1;
        case OP_DIV:
            printf("OP_DIV\n");
            return offset + 1;
        case OP_RETURN:
            printf("OP_RETURN\n");
            return offset + 1;
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}

/* ============================================================================
 * BYTECODE FILE OUTPUT
 * ============================================================================
 */

void write_bytecode_file(CodeGenerator *gen, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s' for writing\n", filename);
        return;
    }
    
    /* Write magic number */
    unsigned int magic = 0x4C534223; /* "LSB#" */
    fwrite(&magic, sizeof(unsigned int), 1, file);
    
    /* Write version */
    unsigned short version = 0x0100; /* 1.0 */
    fwrite(&version, sizeof(unsigned short), 1, file);
    
    /* Write code size */
    fwrite(&gen->chunk->count, sizeof(int), 1, file);
    
    /* Write bytecode */
    fwrite(gen->chunk->code, sizeof(unsigned char), gen->chunk->count, file);
    
    fclose(file);
    printf("Bytecode written to %s (%d bytes)\n", filename, gen->chunk->count);
}


/* ============================================================================
 * EXTENDED CODE GENERATION - ADVANCED FEATURES
 * ============================================================================
 */

/* Register Allocation State */
typedef struct {
    int available_registers[32];
    int register_count;
    ASTNode *register_contents[32];
} RegisterAllocator;

static RegisterAllocator reg_alloc;

void init_register_allocator() {
    reg_alloc.register_count = 32;
    for (int i = 0; i < 32; i++) {
        reg_alloc.available_registers[i] = 1;
        reg_alloc.register_contents[i] = NULL;
    }
}

int allocate_register() {
    for (int i = 0; i < reg_alloc.register_count; i++) {
        if (reg_alloc.available_registers[i]) {
            reg_alloc.available_registers[i] = 0;
            return i;
        }
    }
    return -1;
}

void free_register(int reg) {
    if (reg >= 0 && reg < reg_alloc.register_count) {
        reg_alloc.available_registers[reg] = 1;
        reg_alloc.register_contents[reg] = NULL;
    }
}

/* ============================================================================
 * INSTRUCTION EMISSION
 * ============================================================================
 */

void emit_load_immediate(int reg, int64_t value) {
    emit_byte(0x01);
    emit_byte(reg);
    emit_long(value);
}

void emit_load_register(int dest, int src) {
    emit_byte(0x02);
    emit_byte(dest);
    emit_byte(src);
}

void emit_store_register(int src, int dest) {
    emit_byte(0x03);
    emit_byte(src);
    emit_byte(dest);
}

void emit_add_registers(int dest, int src1, int src2) {
    emit_byte(0x10);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_sub_registers(int dest, int src1, int src2) {
    emit_byte(0x11);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_mul_registers(int dest, int src1, int src2) {
    emit_byte(0x12);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_div_registers(int dest, int src1, int src2) {
    emit_byte(0x13);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_mod_registers(int dest, int src1, int src2) {
    emit_byte(0x14);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_and_registers(int dest, int src1, int src2) {
    emit_byte(0x20);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_or_registers(int dest, int src1, int src2) {
    emit_byte(0x21);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_xor_registers(int dest, int src1, int src2) {
    emit_byte(0x22);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_not_register(int dest, int src) {
    emit_byte(0x23);
    emit_byte(dest);
    emit_byte(src);
}

void emit_shl_registers(int dest, int src1, int src2) {
    emit_byte(0x24);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_shr_registers(int dest, int src1, int src2) {
    emit_byte(0x25);
    emit_byte(dest);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_compare_registers(int src1, int src2) {
    emit_byte(0x30);
    emit_byte(src1);
    emit_byte(src2);
}

void emit_jump(int offset) {
    emit_byte(0x40);
    emit_int(offset);
}

void emit_jump_if_equal(int offset) {
    emit_byte(0x41);
    emit_int(offset);
}

void emit_jump_if_not_equal(int offset) {
    emit_byte(0x42);
    emit_int(offset);
}

void emit_jump_if_less(int offset) {
    emit_byte(0x43);
    emit_int(offset);
}

void emit_jump_if_greater(int offset) {
    emit_byte(0x44);
    emit_int(offset);
}

void emit_jump_if_less_equal(int offset) {
    emit_byte(0x45);
    emit_int(offset);
}

void emit_jump_if_greater_equal(int offset) {
    emit_byte(0x46);
    emit_int(offset);
}

void emit_call(int address) {
    emit_byte(0x50);
    emit_int(address);
}

void emit_return() {
    emit_byte(0x51);
}

void emit_push_register(int reg) {
    emit_byte(0x60);
    emit_byte(reg);
}

void emit_pop_register(int reg) {
    emit_byte(0x61);
    emit_byte(reg);
}

void emit_load_memory(int dest, int address_reg, int offset) {
    emit_byte(0x70);
    emit_byte(dest);
    emit_byte(address_reg);
    emit_int(offset);
}

void emit_store_memory(int src, int address_reg, int offset) {
    emit_byte(0x71);
    emit_byte(src);
    emit_byte(address_reg);
    emit_int(offset);
}

void emit_allocate(int size_reg, int dest_reg) {
    emit_byte(0x80);
    emit_byte(size_reg);
    emit_byte(dest_reg);
}

void emit_deallocate(int ptr_reg) {
    emit_byte(0x81);
    emit_byte(ptr_reg);
}

void emit_syscall(int syscall_num) {
    emit_byte(0x90);
    emit_int(syscall_num);
}

void emit_nop() {
    emit_byte(0x00);
}

void emit_halt() {
    emit_byte(0xFF);
}

/* ============================================================================
 * EXPRESSION CODE GENERATION
 * ============================================================================
 */

int generate_expression_code(ASTNode *node) {
    if (!node) return -1;
    
    switch (node->type) {
        case AST_INT_LITERAL: {
            int reg = allocate_register();
            emit_load_immediate(reg, node->data.int_literal.value);
            return reg;
        }
        
        case AST_BINARY_EXPR: {
            int left_reg = generate_expression_code(node->data.binary_expr.left);
            int right_reg = generate_expression_code(node->data.binary_expr.right);
            int result_reg = allocate_register();
            
            const char *op = node->data.binary_expr.operator;
            if (strcmp(op, "+") == 0) {
                emit_add_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "-") == 0) {
                emit_sub_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "*") == 0) {
                emit_mul_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "/") == 0) {
                emit_div_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "%") == 0) {
                emit_mod_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "&") == 0) {
                emit_and_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "|") == 0) {
                emit_or_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "^") == 0) {
                emit_xor_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, "<<") == 0) {
                emit_shl_registers(result_reg, left_reg, right_reg);
            } else if (strcmp(op, ">>") == 0) {
                emit_shr_registers(result_reg, left_reg, right_reg);
            }
            
            free_register(left_reg);
            free_register(right_reg);
            return result_reg;
        }
        
        case AST_UNARY_EXPR: {
            int operand_reg = generate_expression_code(node->data.unary_expr.operand);
            int result_reg = allocate_register();
            
            const char *op = node->data.unary_expr.operator;
            if (strcmp(op, "-") == 0) {
                emit_load_immediate(result_reg, 0);
                emit_sub_registers(result_reg, result_reg, operand_reg);
            } else if (strcmp(op, "~") == 0) {
                emit_not_register(result_reg, operand_reg);
            }
            
            free_register(operand_reg);
            return result_reg;
        }
        
        default:
            return -1;
    }
}

/* ============================================================================
 * STATEMENT CODE GENERATION
 * ============================================================================
 */

void generate_statement_code(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_EXPR_STMT: {
            int reg = generate_expression_code(node->data.expr_stmt.expression);
            if (reg >= 0) {
                free_register(reg);
            }
            break;
        }
        
        case AST_RETURN_STMT: {
            if (node->data.return_stmt.value) {
                int reg = generate_expression_code(node->data.return_stmt.value);
                emit_load_register(0, reg);
                free_register(reg);
            }
            emit_return();
            break;
        }
        
        case AST_IF_STMT: {
            int cond_reg = generate_expression_code(node->data.if_stmt.condition);
            emit_compare_registers(cond_reg, 0);
            free_register(cond_reg);
            
            int jump_to_else = get_current_position();
            emit_jump_if_equal(0);
            
            generate_statement_code(node->data.if_stmt.then_branch);
            
            int jump_to_end = get_current_position();
            emit_jump(0);
            
            patch_jump(jump_to_else, get_current_position());
            
            if (node->data.if_stmt.else_branch) {
                generate_statement_code(node->data.if_stmt.else_branch);
            }
            
            patch_jump(jump_to_end, get_current_position());
            break;
        }
        
        case AST_WHILE_STMT: {
            int loop_start = get_current_position();
            
            int cond_reg = generate_expression_code(node->data.while_stmt.condition);
            emit_compare_registers(cond_reg, 0);
            free_register(cond_reg);
            
            int jump_to_end = get_current_position();
            emit_jump_if_equal(0);
            
            generate_statement_code(node->data.while_stmt.body);
            
            emit_jump(loop_start);
            
            patch_jump(jump_to_end, get_current_position());
            break;
        }
        
        case AST_BLOCK: {
            if (node->data.block.statements) {
                generate_statement_code(node->data.block.statements);
            }
            break;
        }
        
        case AST_STATEMENT_LIST: {
            for (int i = 0; i < node->data.statement_list.count; i++) {
                generate_statement_code(node->data.statement_list.statements[i]);
            }
            break;
        }
        
        default:
            break;
    }
}

/* ============================================================================
 * FUNCTION CODE GENERATION
 * ============================================================================
 */

void generate_function_code(ASTNode *node) {
    if (!node || node->type != AST_FUNC_DECL) return;
    
    const char *name = node->data.func_decl.name;
    int func_address = get_current_position();
    
    register_function(name, func_address);
    
    emit_push_register(29);
    emit_load_register(29, 30);
    
    if (node->data.func_decl.body) {
        generate_statement_code(node->data.func_decl.body);
    }
    
    emit_load_register(30, 29);
    emit_pop_register(29);
    emit_return();
}

/* ============================================================================
 * OPTIMIZATION PASSES
 * ============================================================================
 */

void peephole_optimization() {
    for (int i = 0; i < bytecode_size - 1; i++) {
        if (bytecode[i] == 0x60 && bytecode[i+1] == 0x61) {
            if (bytecode[i+2] == bytecode[i+3]) {
                bytecode[i] = 0x00;
                bytecode[i+1] = 0x00;
            }
        }
        
        if (bytecode[i] == 0x10) {
            int dest = bytecode[i+1];
            int src1 = bytecode[i+2];
            int src2 = bytecode[i+3];
            
            if (src2 == 0) {
                bytecode[i] = 0x02;
                bytecode[i+1] = dest;
                bytecode[i+2] = src1;
                bytecode[i+3] = 0x00;
            }
        }
        
        if (bytecode[i] == 0x12) {
            int dest = bytecode[i+1];
            int src1 = bytecode[i+2];
            int src2 = bytecode[i+3];
            
            if (src2 == 1) {
                bytecode[i] = 0x02;
                bytecode[i+1] = dest;
                bytecode[i+2] = src1;
                bytecode[i+3] = 0x00;
            }
        }
    }
}

void dead_code_elimination() {
    int reachable[MAX_BYTECODE_SIZE] = {0};
    
    mark_reachable(0, reachable);
    
    for (int i = 0; i < bytecode_size; i++) {
        if (!reachable[i]) {
            bytecode[i] = 0x00;
        }
    }
}

void mark_reachable(int pos, int *reachable) {
    if (pos >= bytecode_size || reachable[pos]) return;
    
    reachable[pos] = 1;
    
    uint8_t opcode = bytecode[pos];
    
    switch (opcode) {
        case 0x40:
            mark_reachable(pos + 5, reachable);
            {
                int offset = *(int*)&bytecode[pos + 1];
                mark_reachable(offset, reachable);
            }
            break;
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
            mark_reachable(pos + 5, reachable);
            {
                int offset = *(int*)&bytecode[pos + 1];
                mark_reachable(offset, reachable);
            }
            break;
        case 0x51:
        case 0xFF:
            break;
        default:
            mark_reachable(pos + get_instruction_size(opcode), reachable);
            break;
    }
}

int get_instruction_size(uint8_t opcode) {
    switch (opcode) {
        case 0x00: return 1;
        case 0x01: return 10;
        case 0x02: return 3;
        case 0x03: return 3;
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: return 4;
        case 0x20: case 0x21: case 0x22: return 4;
        case 0x23: return 3;
        case 0x24: case 0x25: return 4;
        case 0x30: return 3;
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: return 5;
        case 0x50: return 5;
        case 0x51: return 1;
        case 0x60: case 0x61: return 2;
        case 0x70: case 0x71: return 7;
        case 0x80: return 3;
        case 0x81: return 2;
        case 0x90: return 5;
        case 0xFF: return 1;
        default: return 1;
    }
}

/* ============================================================================
 * CONSTANT POOL MANAGEMENT
 * ============================================================================
 */

typedef struct {
    void *data;
    size_t size;
    int type;
} ConstantPoolEntry;

static ConstantPoolEntry constant_pool[1024];
static int constant_pool_size = 0;

int add_constant(void *data, size_t size, int type) {
    if (constant_pool_size >= 1024) return -1;
    
    constant_pool[constant_pool_size].data = malloc(size);
    memcpy(constant_pool[constant_pool_size].data, data, size);
    constant_pool[constant_pool_size].size = size;
    constant_pool[constant_pool_size].type = type;
    
    return constant_pool_size++;
}

void emit_constant_pool() {
    emit_int(constant_pool_size);
    
    for (int i = 0; i < constant_pool_size; i++) {
        emit_int(constant_pool[i].type);
        emit_int(constant_pool[i].size);
        for (size_t j = 0; j < constant_pool[i].size; j++) {
            emit_byte(((uint8_t*)constant_pool[i].data)[j]);
        }
    }
}

void free_constant_pool() {
    for (int i = 0; i < constant_pool_size; i++) {
        free(constant_pool[i].data);
    }
    constant_pool_size = 0;
}

/* ============================================================================
 * DEBUG INFORMATION GENERATION
 * ============================================================================
 */

typedef struct {
    int bytecode_offset;
    int line_number;
    int column_number;
    const char *file_name;
} DebugInfo;

static DebugInfo debug_info[4096];
static int debug_info_count = 0;

void add_debug_info(int offset, int line, int column, const char *file) {
    if (debug_info_count >= 4096) return;
    
    debug_info[debug_info_count].bytecode_offset = offset;
    debug_info[debug_info_count].line_number = line;
    debug_info[debug_info_count].column_number = column;
    debug_info[debug_info_count].file_name = file;
    debug_info_count++;
}

void emit_debug_info() {
    emit_int(debug_info_count);
    
    for (int i = 0; i < debug_info_count; i++) {
        emit_int(debug_info[i].bytecode_offset);
        emit_int(debug_info[i].line_number);
        emit_int(debug_info[i].column_number);
        
        int len = strlen(debug_info[i].file_name);
        emit_int(len);
        for (int j = 0; j < len; j++) {
            emit_byte(debug_info[i].file_name[j]);
        }
    }
}

/* ============================================================================
 * SYMBOL TABLE EMISSION
 * ============================================================================
 */

void emit_symbol_table() {
    emit_int(function_count);
    
    for (int i = 0; i < function_count; i++) {
        int len = strlen(function_table[i].name);
        emit_int(len);
        for (int j = 0; j < len; j++) {
            emit_byte(function_table[i].name[j]);
        }
        emit_int(function_table[i].address);
    }
}

/* ============================================================================
 * MAIN CODE GENERATION ENTRY POINT
 * ============================================================================
 */

void generate_code(ASTNode *root) {
    init_bytecode();
    init_register_allocator();
    
    emit_constant_pool();
    
    generate_statement_code(root);
    
    emit_halt();
    
    peephole_optimization();
    dead_code_elimination();
    
    emit_debug_info();
    emit_symbol_table();
    
    free_constant_pool();
}

void write_bytecode_to_file(const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }
    
    fwrite("LSHARP", 1, 6, f);
    
    uint32_t version = 0x00010000;
    fwrite(&version, sizeof(uint32_t), 1, f);
    
    fwrite(&bytecode_size, sizeof(int), 1, f);
    fwrite(bytecode, 1, bytecode_size, f);
    
    fclose(f);
}

void print_bytecode() {
    printf("\n=== Generated Bytecode ===\n");
    printf("Size: %d bytes\n", bytecode_size);
    
    for (int i = 0; i < bytecode_size; i++) {
        if (i % 16 == 0) {
            printf("\n%04X: ", i);
        }
        printf("%02X ", bytecode[i]);
    }
    
    printf("\n==========================\n");
}

void print_statistics() {
    printf("\n=== Code Generation Statistics ===\n");
    printf("Bytecode size: %d bytes\n", bytecode_size);
    printf("Functions: %d\n", function_count);
    printf("Constants: %d\n", constant_pool_size);
    printf("Debug entries: %d\n", debug_info_count);
    printf("==================================\n");
}

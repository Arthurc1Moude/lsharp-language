/* L# Compiler - IR Header */

#ifndef LSHARP_IR_H
#define LSHARP_IR_H

#include <stdint.h>

typedef enum {
    IR_NOP, IR_LOAD_CONST, IR_LOAD_VAR, IR_STORE_VAR,
    IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_MOD,
    IR_AND, IR_OR, IR_XOR, IR_NOT, IR_SHL, IR_SHR,
    IR_EQ, IR_NE, IR_LT, IR_LE, IR_GT, IR_GE,
    IR_JUMP, IR_JUMP_IF_TRUE, IR_JUMP_IF_FALSE,
    IR_CALL, IR_RETURN, IR_PHI, IR_ALLOCA,
    IR_LOAD, IR_STORE, IR_CAST, IR_SELECT,
    IR_GETELEMENTPTR
} IROpcode;

typedef enum {
    IR_TYPE_VOID, IR_TYPE_I1, IR_TYPE_I8, IR_TYPE_I16,
    IR_TYPE_I32, IR_TYPE_I64, IR_TYPE_F32, IR_TYPE_F64,
    IR_TYPE_PTR, IR_TYPE_ARRAY, IR_TYPE_STRUCT
} IRType;

typedef struct IRValue IRValue;
typedef struct IRInstruction IRInstruction;
typedef struct BasicBlock BasicBlock;
typedef struct IRFunction IRFunction;
typedef struct IRModule IRModule;

IRValue* create_ir_value(IRType type);
IRValue* create_ir_const_int(int64_t val);
IRValue* create_ir_const_float(double val);

IRInstruction* create_ir_instruction(IROpcode opcode);
BasicBlock* create_basic_block(const char *label);

IRValue* ir_build_add(IRValue *lhs, IRValue *rhs);
IRValue* ir_build_sub(IRValue *lhs, IRValue *rhs);
IRValue* ir_build_mul(IRValue *lhs, IRValue *rhs);
IRValue* ir_build_div(IRValue *lhs, IRValue *rhs);
IRValue* ir_build_load(IRValue *ptr);
void ir_build_store(IRValue *value, IRValue *ptr);
void ir_build_return(IRValue *value);

void run_ir_optimization_passes(IRFunction *func);
void print_ir_module(IRModule *module);

#endif /* LSHARP_IR_H */

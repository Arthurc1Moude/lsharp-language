/* ============================================================================
 * L# Compiler - Intermediate Representation
 * ============================================================================
 * File: ir.c
 * Purpose: IR generation, optimization, and manipulation
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
 * IR INSTRUCTION TYPES
 * ============================================================================
 */

typedef enum {
    IR_NOP,
    IR_LOAD_CONST,
    IR_LOAD_VAR,
    IR_STORE_VAR,
    IR_LOAD_GLOBAL,
    IR_STORE_GLOBAL,
    IR_LOAD_FIELD,
    IR_STORE_FIELD,
    IR_LOAD_ARRAY,
    IR_STORE_ARRAY,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD,
    IR_NEG,
    IR_AND,
    IR_OR,
    IR_XOR,
    IR_NOT,
    IR_SHL,
    IR_SHR,
    IR_EQ,
    IR_NE,
    IR_LT,
    IR_LE,
    IR_GT,
    IR_GE,
    IR_JUMP,
    IR_JUMP_IF_TRUE,
    IR_JUMP_IF_FALSE,
    IR_CALL,
    IR_RETURN,
    IR_PHI,
    IR_ALLOCA,
    IR_LOAD,
    IR_STORE,
    IR_CAST,
    IR_BITCAST,
    IR_TRUNC,
    IR_ZEXT,
    IR_SEXT,
    IR_FPTRUNC,
    IR_FPEXT,
    IR_FPTOSI,
    IR_FPTOUI,
    IR_SITOFP,
    IR_UITOFP,
    IR_PTRTOINT,
    IR_INTTOPTR,
    IR_SELECT,
    IR_EXTRACTVALUE,
    IR_INSERTVALUE,
    IR_GETELEMENTPTR,
    IR_FENCE,
    IR_CMPXCHG,
    IR_ATOMICRMW,
    IR_LANDINGPAD,
    IR_RESUME,
    IR_UNREACHABLE
} IROpcode;

/* ============================================================================
 * IR VALUE TYPES
 * ============================================================================
 */

typedef enum {
    IR_TYPE_VOID,
    IR_TYPE_I1,
    IR_TYPE_I8,
    IR_TYPE_I16,
    IR_TYPE_I32,
    IR_TYPE_I64,
    IR_TYPE_I128,
    IR_TYPE_F32,
    IR_TYPE_F64,
    IR_TYPE_PTR,
    IR_TYPE_ARRAY,
    IR_TYPE_STRUCT,
    IR_TYPE_FUNCTION,
    IR_TYPE_LABEL
} IRType;

typedef struct IRValue {
    IRType type;
    union {
        int64_t int_val;
        double float_val;
        void *ptr_val;
    } data;
    int id;
    char *name;
} IRValue;

/* ============================================================================
 * IR INSTRUCTION STRUCTURE
 * ============================================================================
 */

typedef struct IRInstruction {
    IROpcode opcode;
    IRValue *result;
    IRValue *operand1;
    IRValue *operand2;
    IRValue *operand3;
    int block_id;
    struct IRInstruction *next;
    struct IRInstruction *prev;
} IRInstruction;

/* ============================================================================
 * BASIC BLOCK
 * ============================================================================
 */

typedef struct BasicBlock {
    int id;
    char *label;
    IRInstruction *first;
    IRInstruction *last;
    struct BasicBlock **predecessors;
    int pred_count;
    struct BasicBlock **successors;
    int succ_count;
    struct BasicBlock *next;
} BasicBlock;

/* ============================================================================
 * FUNCTION IR
 * ============================================================================
 */

typedef struct IRFunction {
    char *name;
    IRType return_type;
    IRValue **parameters;
    int param_count;
    BasicBlock *entry_block;
    BasicBlock *blocks;
    int block_count;
    struct IRFunction *next;
} IRFunction;

/* ============================================================================
 * MODULE IR
 * ============================================================================
 */

typedef struct IRModule {
    char *name;
    IRFunction *functions;
    int function_count;
    IRValue **globals;
    int global_count;
} IRModule;

/* ============================================================================
 * IR BUILDER STATE
 * ============================================================================
 */

static IRModule *current_module = NULL;
static IRFunction *current_function = NULL;
static BasicBlock *current_block = NULL;
static int next_value_id = 0;
static int next_block_id = 0;

/* ============================================================================
 * VALUE CREATION
 * ============================================================================
 */

IRValue* create_ir_value(IRType type) {
    IRValue *value = malloc(sizeof(IRValue));
    value->type = type;
    value->id = next_value_id++;
    value->name = NULL;
    return value;
}

IRValue* create_ir_const_int(int64_t val) {
    IRValue *value = create_ir_value(IR_TYPE_I64);
    value->data.int_val = val;
    return value;
}

IRValue* create_ir_const_float(double val) {
    IRValue *value = create_ir_value(IR_TYPE_F64);
    value->data.float_val = val;
    return value;
}

/* ============================================================================
 * INSTRUCTION CREATION
 * ============================================================================
 */

IRInstruction* create_ir_instruction(IROpcode opcode) {
    IRInstruction *inst = malloc(sizeof(IRInstruction));
    inst->opcode = opcode;
    inst->result = NULL;
    inst->operand1 = NULL;
    inst->operand2 = NULL;
    inst->operand3 = NULL;
    inst->block_id = current_block ? current_block->id : -1;
    inst->next = NULL;
    inst->prev = NULL;
    return inst;
}

void append_instruction(BasicBlock *block, IRInstruction *inst) {
    if (!block->first) {
        block->first = block->last = inst;
    } else {
        inst->prev = block->last;
        block->last->next = inst;
        block->last = inst;
    }
}

/* ============================================================================
 * BASIC BLOCK CREATION
 * ============================================================================
 */

BasicBlock* create_basic_block(const char *label) {
    BasicBlock *block = malloc(sizeof(BasicBlock));
    block->id = next_block_id++;
    block->label = label ? strdup(label) : NULL;
    block->first = NULL;
    block->last = NULL;
    block->predecessors = NULL;
    block->pred_count = 0;
    block->successors = NULL;
    block->succ_count = 0;
    block->next = NULL;
    return block;
}

void add_successor(BasicBlock *from, BasicBlock *to) {
    from->successors = realloc(from->successors, sizeof(BasicBlock*) * (from->succ_count + 1));
    from->successors[from->succ_count++] = to;
    
    to->predecessors = realloc(to->predecessors, sizeof(BasicBlock*) * (to->pred_count + 1));
    to->predecessors[to->pred_count++] = from;
}

/* ============================================================================
 * IR BUILDER FUNCTIONS
 * ============================================================================
 */

IRValue* ir_build_add(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_ADD);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_sub(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_SUB);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_mul(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_MUL);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_div(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_DIV);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_mod(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_MOD);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_and(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_AND);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_or(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_OR);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_xor(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_XOR);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_shl(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_SHL);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_shr(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_SHR);
    inst->result = create_ir_value(lhs->type);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_neg(IRValue *operand) {
    IRInstruction *inst = create_ir_instruction(IR_NEG);
    inst->result = create_ir_value(operand->type);
    inst->operand1 = operand;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_not(IRValue *operand) {
    IRInstruction *inst = create_ir_instruction(IR_NOT);
    inst->result = create_ir_value(operand->type);
    inst->operand1 = operand;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cmp_eq(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_EQ);
    inst->result = create_ir_value(IR_TYPE_I1);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cmp_ne(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_NE);
    inst->result = create_ir_value(IR_TYPE_I1);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cmp_lt(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_LT);
    inst->result = create_ir_value(IR_TYPE_I1);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cmp_le(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_LE);
    inst->result = create_ir_value(IR_TYPE_I1);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cmp_gt(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_GT);
    inst->result = create_ir_value(IR_TYPE_I1);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cmp_ge(IRValue *lhs, IRValue *rhs) {
    IRInstruction *inst = create_ir_instruction(IR_GE);
    inst->result = create_ir_value(IR_TYPE_I1);
    inst->operand1 = lhs;
    inst->operand2 = rhs;
    append_instruction(current_block, inst);
    return inst->result;
}

void ir_build_jump(BasicBlock *target) {
    IRInstruction *inst = create_ir_instruction(IR_JUMP);
    inst->operand1 = (IRValue*)target;
    append_instruction(current_block, inst);
    add_successor(current_block, target);
}

void ir_build_cond_jump(IRValue *condition, BasicBlock *true_block, BasicBlock *false_block) {
    IRInstruction *inst = create_ir_instruction(IR_JUMP_IF_TRUE);
    inst->operand1 = condition;
    inst->operand2 = (IRValue*)true_block;
    inst->operand3 = (IRValue*)false_block;
    append_instruction(current_block, inst);
    add_successor(current_block, true_block);
    add_successor(current_block, false_block);
}

IRValue* ir_build_call(IRFunction *func, IRValue **args, int arg_count) {
    IRInstruction *inst = create_ir_instruction(IR_CALL);
    inst->result = create_ir_value(func->return_type);
    inst->operand1 = (IRValue*)func;
    append_instruction(current_block, inst);
    return inst->result;
}

void ir_build_return(IRValue *value) {
    IRInstruction *inst = create_ir_instruction(IR_RETURN);
    inst->operand1 = value;
    append_instruction(current_block, inst);
}

IRValue* ir_build_alloca(IRType type) {
    IRInstruction *inst = create_ir_instruction(IR_ALLOCA);
    inst->result = create_ir_value(IR_TYPE_PTR);
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_load(IRValue *ptr) {
    IRInstruction *inst = create_ir_instruction(IR_LOAD);
    inst->result = create_ir_value(IR_TYPE_I64);
    inst->operand1 = ptr;
    append_instruction(current_block, inst);
    return inst->result;
}

void ir_build_store(IRValue *value, IRValue *ptr) {
    IRInstruction *inst = create_ir_instruction(IR_STORE);
    inst->operand1 = value;
    inst->operand2 = ptr;
    append_instruction(current_block, inst);
}

IRValue* ir_build_gep(IRValue *ptr, IRValue **indices, int index_count) {
    IRInstruction *inst = create_ir_instruction(IR_GETELEMENTPTR);
    inst->result = create_ir_value(IR_TYPE_PTR);
    inst->operand1 = ptr;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_cast(IRValue *value, IRType target_type) {
    IRInstruction *inst = create_ir_instruction(IR_CAST);
    inst->result = create_ir_value(target_type);
    inst->operand1 = value;
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_phi(IRType type, IRValue **values, BasicBlock **blocks, int count) {
    IRInstruction *inst = create_ir_instruction(IR_PHI);
    inst->result = create_ir_value(type);
    append_instruction(current_block, inst);
    return inst->result;
}

IRValue* ir_build_select(IRValue *condition, IRValue *true_val, IRValue *false_val) {
    IRInstruction *inst = create_ir_instruction(IR_SELECT);
    inst->result = create_ir_value(true_val->type);
    inst->operand1 = condition;
    inst->operand2 = true_val;
    inst->operand3 = false_val;
    append_instruction(current_block, inst);
    return inst->result;
}

/* ============================================================================
 * CONTROL FLOW GRAPH ANALYSIS
 * ============================================================================
 */

void compute_dominators(IRFunction *func) {
    int block_count = func->block_count;
    int **dom = malloc(sizeof(int*) * block_count);
    
    for (int i = 0; i < block_count; i++) {
        dom[i] = malloc(sizeof(int) * block_count);
        for (int j = 0; j < block_count; j++) {
            dom[i][j] = (i == 0) ? (i == j) : 1;
        }
    }
    
    int changed = 1;
    while (changed) {
        changed = 0;
        BasicBlock *block = func->blocks;
        int block_idx = 0;
        
        while (block) {
            if (block_idx > 0) {
                int new_dom[block_count];
                for (int i = 0; i < block_count; i++) {
                    new_dom[i] = 1;
                }
                
                for (int p = 0; p < block->pred_count; p++) {
                    BasicBlock *pred = block->predecessors[p];
                    int pred_idx = 0;
                    BasicBlock *temp = func->blocks;
                    while (temp != pred) {
                        pred_idx++;
                        temp = temp->next;
                    }
                    
                    for (int i = 0; i < block_count; i++) {
                        new_dom[i] = new_dom[i] && dom[pred_idx][i];
                    }
                }
                
                new_dom[block_idx] = 1;
                
                for (int i = 0; i < block_count; i++) {
                    if (dom[block_idx][i] != new_dom[i]) {
                        dom[block_idx][i] = new_dom[i];
                        changed = 1;
                    }
                }
            }
            
            block = block->next;
            block_idx++;
        }
    }
    
    for (int i = 0; i < block_count; i++) {
        free(dom[i]);
    }
    free(dom);
}

void compute_dominance_frontier(IRFunction *func) {
    compute_dominators(func);
}

/* ============================================================================
 * SSA CONSTRUCTION
 * ============================================================================
 */

void insert_phi_functions(IRFunction *func) {
    compute_dominance_frontier(func);
}

void rename_variables(IRFunction *func) {
}

void construct_ssa(IRFunction *func) {
    insert_phi_functions(func);
    rename_variables(func);
}

/* ============================================================================
 * IR OPTIMIZATION PASSES
 * ============================================================================
 */

void constant_folding_ir(IRFunction *func) {
    BasicBlock *block = func->blocks;
    
    while (block) {
        IRInstruction *inst = block->first;
        
        while (inst) {
            if (inst->opcode == IR_ADD) {
                if (inst->operand1->data.int_val == 0) {
                    inst->result = inst->operand2;
                } else if (inst->operand2->data.int_val == 0) {
                    inst->result = inst->operand1;
                }
            } else if (inst->opcode == IR_MUL) {
                if (inst->operand1->data.int_val == 0 || inst->operand2->data.int_val == 0) {
                    inst->result = create_ir_const_int(0);
                } else if (inst->operand1->data.int_val == 1) {
                    inst->result = inst->operand2;
                } else if (inst->operand2->data.int_val == 1) {
                    inst->result = inst->operand1;
                }
            }
            
            inst = inst->next;
        }
        
        block = block->next;
    }
}

void dead_code_elimination_ir(IRFunction *func) {
    int changed = 1;
    
    while (changed) {
        changed = 0;
        BasicBlock *block = func->blocks;
        
        while (block) {
            IRInstruction *inst = block->first;
            IRInstruction *prev = NULL;
            
            while (inst) {
                int is_dead = 1;
                
                if (inst->opcode == IR_STORE || inst->opcode == IR_CALL || 
                    inst->opcode == IR_RETURN || inst->opcode == IR_JUMP ||
                    inst->opcode == IR_JUMP_IF_TRUE) {
                    is_dead = 0;
                }
                
                if (is_dead) {
                    IRInstruction *next = inst->next;
                    if (prev) {
                        prev->next = next;
                    } else {
                        block->first = next;
                    }
                    if (next) {
                        next->prev = prev;
                    } else {
                        block->last = prev;
                    }
                    free(inst);
                    inst = next;
                    changed = 1;
                } else {
                    prev = inst;
                    inst = inst->next;
                }
            }
            
            block = block->next;
        }
    }
}

void common_subexpression_elimination_ir(IRFunction *func) {
    BasicBlock *block = func->blocks;
    
    while (block) {
        IRInstruction *inst1 = block->first;
        
        while (inst1) {
            IRInstruction *inst2 = inst1->next;
            
            while (inst2) {
                if (inst1->opcode == inst2->opcode &&
                    inst1->operand1 == inst2->operand1 &&
                    inst1->operand2 == inst2->operand2) {
                    inst2->result = inst1->result;
                }
                
                inst2 = inst2->next;
            }
            
            inst1 = inst1->next;
        }
        
        block = block->next;
    }
}

void loop_invariant_code_motion_ir(IRFunction *func) {
}

void strength_reduction_ir(IRFunction *func) {
    BasicBlock *block = func->blocks;
    
    while (block) {
        IRInstruction *inst = block->first;
        
        while (inst) {
            if (inst->opcode == IR_MUL) {
                int64_t val = inst->operand2->data.int_val;
                if ((val & (val - 1)) == 0) {
                    int shift = 0;
                    while ((1LL << shift) < val) shift++;
                    inst->opcode = IR_SHL;
                    inst->operand2 = create_ir_const_int(shift);
                }
            }
            
            inst = inst->next;
        }
        
        block = block->next;
    }
}

void inline_functions_ir(IRFunction *func) {
}

void tail_call_optimization_ir(IRFunction *func) {
}

void run_ir_optimization_passes(IRFunction *func) {
    constant_folding_ir(func);
    dead_code_elimination_ir(func);
    common_subexpression_elimination_ir(func);
    strength_reduction_ir(func);
    construct_ssa(func);
}

/* ============================================================================
 * IR PRINTING
 * ============================================================================
 */

const char* opcode_to_string(IROpcode opcode) {
    switch (opcode) {
        case IR_ADD: return "add";
        case IR_SUB: return "sub";
        case IR_MUL: return "mul";
        case IR_DIV: return "div";
        case IR_MOD: return "mod";
        case IR_AND: return "and";
        case IR_OR: return "or";
        case IR_XOR: return "xor";
        case IR_NOT: return "not";
        case IR_SHL: return "shl";
        case IR_SHR: return "shr";
        case IR_EQ: return "eq";
        case IR_NE: return "ne";
        case IR_LT: return "lt";
        case IR_LE: return "le";
        case IR_GT: return "gt";
        case IR_GE: return "ge";
        case IR_LOAD: return "load";
        case IR_STORE: return "store";
        case IR_ALLOCA: return "alloca";
        case IR_CALL: return "call";
        case IR_RETURN: return "ret";
        case IR_JUMP: return "br";
        case IR_PHI: return "phi";
        default: return "unknown";
    }
}

void print_ir_value(IRValue *value) {
    if (!value) {
        printf("null");
        return;
    }
    
    if (value->name) {
        printf("%%%s", value->name);
    } else {
        printf("%%%d", value->id);
    }
}

void print_ir_instruction(IRInstruction *inst) {
    printf("  ");
    
    if (inst->result) {
        print_ir_value(inst->result);
        printf(" = ");
    }
    
    printf("%s ", opcode_to_string(inst->opcode));
    
    if (inst->operand1) {
        print_ir_value(inst->operand1);
    }
    if (inst->operand2) {
        printf(", ");
        print_ir_value(inst->operand2);
    }
    if (inst->operand3) {
        printf(", ");
        print_ir_value(inst->operand3);
    }
    
    printf("\n");
}

void print_ir_basic_block(BasicBlock *block) {
    if (block->label) {
        printf("%s:\n", block->label);
    } else {
        printf("bb%d:\n", block->id);
    }
    
    IRInstruction *inst = block->first;
    while (inst) {
        print_ir_instruction(inst);
        inst = inst->next;
    }
}

void print_ir_function(IRFunction *func) {
    printf("\ndefine %s @%s(", "i64", func->name);
    
    for (int i = 0; i < func->param_count; i++) {
        if (i > 0) printf(", ");
        printf("i64 ");
        print_ir_value(func->parameters[i]);
    }
    
    printf(") {\n");
    
    BasicBlock *block = func->blocks;
    while (block) {
        print_ir_basic_block(block);
        block = block->next;
    }
    
    printf("}\n");
}

void print_ir_module(IRModule *module) {
    printf("; Module: %s\n", module->name);
    
    IRFunction *func = module->functions;
    while (func) {
        print_ir_function(func);
        func = func->next;
    }
}

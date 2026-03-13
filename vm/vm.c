/* ============================================================================
 * L# Programming Language - Virtual Machine Implementation
 * ============================================================================
 * File: vm.c
 * Purpose: Bytecode interpreter and execution engine
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * BYTECODE INSTRUCTION SET
 * ============================================================================
 */

typedef enum {
    /* Stack Operations */
    OP_PUSH,            /* Push constant onto stack */
    OP_POP,             /* Pop value from stack */
    OP_DUP,             /* Duplicate top of stack */
    OP_SWAP,            /* Swap top two stack values */
    
    /* Arithmetic Operations */
    OP_ADD,             /* Addition */
    OP_SUB,             /* Subtraction */
    OP_MUL,             /* Multiplication */
    OP_DIV,             /* Division */
    OP_MOD,             /* Modulo */
    OP_NEG,             /* Negation */
    OP_POW,             /* Power */
    
    /* Bitwise Operations */
    OP_AND,             /* Bitwise AND */
    OP_OR,              /* Bitwise OR */
    OP_XOR,             /* Bitwise XOR */
    OP_NOT,             /* Bitwise NOT */
    OP_SHL,             /* Shift left */
    OP_SHR,             /* Shift right */
    
    /* Comparison Operations */
    OP_EQ,              /* Equal */
    OP_NE,              /* Not equal */
    OP_LT,              /* Less than */
    OP_LE,              /* Less than or equal */
    OP_GT,              /* Greater than */
    OP_GE,              /* Greater than or equal */
    
    /* Logical Operations */
    OP_LAND,            /* Logical AND */
    OP_LOR,             /* Logical OR */
    OP_LNOT,            /* Logical NOT */
    
    /* Memory Operations */
    OP_LOAD_LOCAL,      /* Load local variable */
    OP_STORE_LOCAL,     /* Store local variable */
    OP_LOAD_GLOBAL,     /* Load global variable */
    OP_STORE_GLOBAL,    /* Store global variable */
    OP_LOAD_FIELD,      /* Load object field */
    OP_STORE_FIELD,     /* Store object field */
    OP_LOAD_INDEX,      /* Load array element */
    OP_STORE_INDEX,     /* Store array element */
    
    /* Control Flow */
    OP_JUMP,            /* Unconditional jump */
    OP_JUMP_IF_TRUE,    /* Jump if true */
    OP_JUMP_IF_FALSE,   /* Jump if false */
    OP_CALL,            /* Function call */
    OP_RETURN,          /* Return from function */
    
    /* Object Operations */
    OP_NEW_OBJECT,      /* Create new object */
    OP_NEW_ARRAY,       /* Create new array */
    OP_NEW_STRING,      /* Create new string */
    OP_NEW_CLOSURE,     /* Create closure */
    
    /* Type Operations */
    OP_CAST,            /* Type cast */
    OP_INSTANCEOF,      /* Instance check */
    OP_TYPEOF,          /* Get type */
    
    /* Special Operations */
    OP_PRINT,           /* Print value */
    OP_HALT,            /* Stop execution */
    OP_NOP              /* No operation */
} Opcode;

/* ============================================================================
 * VALUE REPRESENTATION
 * ============================================================================
 */

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_STRING,
    VAL_ARRAY,
    VAL_OBJECT,
    VAL_FUNCTION,
    VAL_NULL
} ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t int_val;
        double float_val;
        bool bool_val;
        char *string_val;
        void *ptr_val;
    } as;
} Value;

/* Value constructors */
Value make_int(int64_t val) {
    Value v;
    v.type = VAL_INT;
    v.as.int_val = val;
    return v;
}

Value make_float(double val) {
    Value v;
    v.type = VAL_FLOAT;
    v.as.float_val = val;
    return v;
}

Value make_bool(bool val) {
    Value v;
    v.type = VAL_BOOL;
    v.as.bool_val = val;
    return v;
}

Value make_string(const char *val) {
    Value v;
    v.type = VAL_STRING;
    v.as.string_val = strdup(val);
    return v;
}

Value make_null() {
    Value v;
    v.type = VAL_NULL;
    v.as.ptr_val = NULL;
    return v;
}

/* ============================================================================
 * VIRTUAL MACHINE STATE
 * ============================================================================
 */

#define STACK_SIZE 4096
#define CALL_STACK_SIZE 256
#define GLOBAL_SIZE 1024

typedef struct {
    uint8_t *bytecode;
    size_t bytecode_size;
    size_t ip;              /* Instruction pointer */
    
    Value stack[STACK_SIZE];
    int sp;                 /* Stack pointer */
    
    Value globals[GLOBAL_SIZE];
    
    struct CallFrame {
        size_t return_address;
        int base_pointer;
    } call_stack[CALL_STACK_SIZE];
    int call_sp;
    
    bool running;
    bool debug_mode;
} VM;

static VM vm;

/* ============================================================================
 * VM INITIALIZATION
 * ============================================================================
 */

void vm_init() {
    vm.bytecode = NULL;
    vm.bytecode_size = 0;
    vm.ip = 0;
    vm.sp = 0;
    vm.call_sp = 0;
    vm.running = false;
    vm.debug_mode = false;
    
    for (int i = 0; i < GLOBAL_SIZE; i++) {
        vm.globals[i] = make_null();
    }
}

void vm_load_bytecode(uint8_t *code, size_t size) {
    vm.bytecode = code;
    vm.bytecode_size = size;
    vm.ip = 0;
}

/* ============================================================================
 * STACK OPERATIONS
 * ============================================================================
 */

void push(Value val) {
    if (vm.sp >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    vm.stack[vm.sp++] = val;
}

Value pop() {
    if (vm.sp <= 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(1);
    }
    return vm.stack[--vm.sp];
}

Value peek(int distance) {
    if (vm.sp - distance - 1 < 0) {
        fprintf(stderr, "Stack peek out of bounds\n");
        exit(1);
    }
    return vm.stack[vm.sp - distance - 1];
}

/* ============================================================================
 * BYTECODE READING
 * ============================================================================
 */

uint8_t read_byte() {
    if (vm.ip >= vm.bytecode_size) {
        fprintf(stderr, "Bytecode read out of bounds\n");
        exit(1);
    }
    return vm.bytecode[vm.ip++];
}

uint16_t read_short() {
    uint16_t val = (vm.bytecode[vm.ip] << 8) | vm.bytecode[vm.ip + 1];
    vm.ip += 2;
    return val;
}

uint32_t read_int() {
    uint32_t val = (vm.bytecode[vm.ip] << 24) | 
                   (vm.bytecode[vm.ip + 1] << 16) |
                   (vm.bytecode[vm.ip + 2] << 8) | 
                   vm.bytecode[vm.ip + 3];
    vm.ip += 4;
    return val;
}

int64_t read_long() {
    int64_t val = 0;
    for (int i = 0; i < 8; i++) {
        val = (val << 8) | vm.bytecode[vm.ip++];
    }
    return val;
}

double read_double() {
    union {
        uint64_t i;
        double d;
    } u;
    u.i = read_long();
    return u.d;
}

/* ============================================================================
 * ARITHMETIC OPERATIONS
 * ============================================================================
 */

void op_add() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val + b.as.int_val));
    } else if (a.type == VAL_FLOAT || b.type == VAL_FLOAT) {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_float(av + bv));
    } else if (a.type == VAL_STRING && b.type == VAL_STRING) {
        size_t len = strlen(a.as.string_val) + strlen(b.as.string_val) + 1;
        char *result = malloc(len);
        strcpy(result, a.as.string_val);
        strcat(result, b.as.string_val);
        push(make_string(result));
        free(result);
    } else {
        fprintf(stderr, "Type error in addition\n");
        exit(1);
    }
}

void op_sub() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val - b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_float(av - bv));
    }
}

void op_mul() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val * b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_float(av * bv));
    }
}

void op_div() {
    Value b = pop();
    Value a = pop();
    
    if (b.type == VAL_INT && b.as.int_val == 0) {
        fprintf(stderr, "Division by zero\n");
        exit(1);
    }
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val / b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_float(av / bv));
    }
}

void op_mod() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val % b.as.int_val));
    } else {
        fprintf(stderr, "Modulo requires integer operands\n");
        exit(1);
    }
}

void op_neg() {
    Value a = pop();
    
    if (a.type == VAL_INT) {
        push(make_int(-a.as.int_val));
    } else if (a.type == VAL_FLOAT) {
        push(make_float(-a.as.float_val));
    } else {
        fprintf(stderr, "Type error in negation\n");
        exit(1);
    }
}

/* ============================================================================
 * BITWISE OPERATIONS
 * ============================================================================
 */

void op_and() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val & b.as.int_val));
    } else {
        fprintf(stderr, "Bitwise AND requires integer operands\n");
        exit(1);
    }
}

void op_or() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val | b.as.int_val));
    } else {
        fprintf(stderr, "Bitwise OR requires integer operands\n");
        exit(1);
    }
}

void op_xor() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val ^ b.as.int_val));
    } else {
        fprintf(stderr, "Bitwise XOR requires integer operands\n");
        exit(1);
    }
}

void op_not() {
    Value a = pop();
    
    if (a.type == VAL_INT) {
        push(make_int(~a.as.int_val));
    } else {
        fprintf(stderr, "Bitwise NOT requires integer operand\n");
        exit(1);
    }
}

void op_shl() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val << b.as.int_val));
    } else {
        fprintf(stderr, "Shift left requires integer operands\n");
        exit(1);
    }
}

void op_shr() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_int(a.as.int_val >> b.as.int_val));
    } else {
        fprintf(stderr, "Shift right requires integer operands\n");
        exit(1);
    }
}

/* ============================================================================
 * COMPARISON OPERATIONS
 * ============================================================================
 */

void op_eq() {
    Value b = pop();
    Value a = pop();
    
    if (a.type != b.type) {
        push(make_bool(false));
        return;
    }
    
    switch (a.type) {
        case VAL_INT:
            push(make_bool(a.as.int_val == b.as.int_val));
            break;
        case VAL_FLOAT:
            push(make_bool(a.as.float_val == b.as.float_val));
            break;
        case VAL_BOOL:
            push(make_bool(a.as.bool_val == b.as.bool_val));
            break;
        case VAL_STRING:
            push(make_bool(strcmp(a.as.string_val, b.as.string_val) == 0));
            break;
        case VAL_NULL:
            push(make_bool(true));
            break;
        default:
            push(make_bool(false));
    }
}

void op_ne() {
    op_eq();
    Value result = pop();
    push(make_bool(!result.as.bool_val));
}

void op_lt() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_bool(a.as.int_val < b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_bool(av < bv));
    }
}

void op_le() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_bool(a.as.int_val <= b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_bool(av <= bv));
    }
}

void op_gt() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_bool(a.as.int_val > b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_bool(av > bv));
    }
}

void op_ge() {
    Value b = pop();
    Value a = pop();
    
    if (a.type == VAL_INT && b.type == VAL_INT) {
        push(make_bool(a.as.int_val >= b.as.int_val));
    } else {
        double av = (a.type == VAL_FLOAT) ? a.as.float_val : a.as.int_val;
        double bv = (b.type == VAL_FLOAT) ? b.as.float_val : b.as.int_val;
        push(make_bool(av >= bv));
    }
}

/* ============================================================================
 * LOGICAL OPERATIONS
 * ============================================================================
 */

void op_land() {
    Value b = pop();
    Value a = pop();
    
    bool av = (a.type == VAL_BOOL) ? a.as.bool_val : (a.type != VAL_NULL);
    bool bv = (b.type == VAL_BOOL) ? b.as.bool_val : (b.type != VAL_NULL);
    
    push(make_bool(av && bv));
}

void op_lor() {
    Value b = pop();
    Value a = pop();
    
    bool av = (a.type == VAL_BOOL) ? a.as.bool_val : (a.type != VAL_NULL);
    bool bv = (b.type == VAL_BOOL) ? b.as.bool_val : (b.type != VAL_NULL);
    
    push(make_bool(av || bv));
}

void op_lnot() {
    Value a = pop();
    
    bool av = (a.type == VAL_BOOL) ? a.as.bool_val : (a.type != VAL_NULL);
    
    push(make_bool(!av));
}

/* ============================================================================
 * MEMORY OPERATIONS
 * ============================================================================
 */

void op_load_local() {
    uint16_t index = read_short();
    int bp = (vm.call_sp > 0) ? vm.call_stack[vm.call_sp - 1].base_pointer : 0;
    push(vm.stack[bp + index]);
}

void op_store_local() {
    uint16_t index = read_short();
    int bp = (vm.call_sp > 0) ? vm.call_stack[vm.call_sp - 1].base_pointer : 0;
    vm.stack[bp + index] = pop();
}

void op_load_global() {
    uint16_t index = read_short();
    if (index >= GLOBAL_SIZE) {
        fprintf(stderr, "Global index out of bounds\n");
        exit(1);
    }
    push(vm.globals[index]);
}

void op_store_global() {
    uint16_t index = read_short();
    if (index >= GLOBAL_SIZE) {
        fprintf(stderr, "Global index out of bounds\n");
        exit(1);
    }
    vm.globals[index] = pop();
}

/* ============================================================================
 * CONTROL FLOW
 * ============================================================================
 */

void op_jump() {
    uint32_t offset = read_int();
    vm.ip = offset;
}

void op_jump_if_true() {
    uint32_t offset = read_int();
    Value condition = pop();
    
    bool cond = (condition.type == VAL_BOOL) ? condition.as.bool_val : 
                (condition.type != VAL_NULL);
    
    if (cond) {
        vm.ip = offset;
    }
}

void op_jump_if_false() {
    uint32_t offset = read_int();
    Value condition = pop();
    
    bool cond = (condition.type == VAL_BOOL) ? condition.as.bool_val : 
                (condition.type != VAL_NULL);
    
    if (!cond) {
        vm.ip = offset;
    }
}

void op_call() {
    uint8_t arg_count = read_byte();
    
    if (vm.call_sp >= CALL_STACK_SIZE) {
        fprintf(stderr, "Call stack overflow\n");
        exit(1);
    }
    
    vm.call_stack[vm.call_sp].return_address = vm.ip;
    vm.call_stack[vm.call_sp].base_pointer = vm.sp - arg_count;
    vm.call_sp++;
    
    /* Function address should be on stack */
    Value func = pop();
    if (func.type != VAL_FUNCTION) {
        fprintf(stderr, "Attempt to call non-function\n");
        exit(1);
    }
    
    vm.ip = (size_t)func.as.ptr_val;
}

void op_return() {
    if (vm.call_sp <= 0) {
        vm.running = false;
        return;
    }
    
    Value return_value = pop();
    
    vm.call_sp--;
    vm.ip = vm.call_stack[vm.call_sp].return_address;
    vm.sp = vm.call_stack[vm.call_sp].base_pointer;
    
    push(return_value);
}

/* ============================================================================
 * PRINT OPERATION
 * ============================================================================
 */

void op_print() {
    Value val = pop();
    
    switch (val.type) {
        case VAL_INT:
            printf("%lld", val.as.int_val);
            break;
        case VAL_FLOAT:
            printf("%f", val.as.float_val);
            break;
        case VAL_BOOL:
            printf("%s", val.as.bool_val ? "true" : "false");
            break;
        case VAL_STRING:
            printf("%s", val.as.string_val);
            break;
        case VAL_NULL:
            printf("null");
            break;
        default:
            printf("<object>");
    }
}

/* ============================================================================
 * MAIN EXECUTION LOOP
 * ============================================================================
 */

void vm_run() {
    vm.running = true;
    vm.ip = 0;
    
    while (vm.running && vm.ip < vm.bytecode_size) {
        if (vm.debug_mode) {
            printf("IP: %zu, SP: %d, Opcode: %d\n", vm.ip, vm.sp, vm.bytecode[vm.ip]);
        }
        
        uint8_t instruction = read_byte();
        
        switch (instruction) {
            case OP_PUSH: {
                uint8_t type = read_byte();
                switch (type) {
                    case 0: push(make_int(read_long())); break;
                    case 1: push(make_float(read_double())); break;
                    case 2: push(make_bool(read_byte())); break;
                    case 3: push(make_null()); break;
                }
                break;
            }
            case OP_POP: pop(); break;
            case OP_DUP: push(peek(0)); break;
            case OP_SWAP: {
                Value a = pop();
                Value b = pop();
                push(a);
                push(b);
                break;
            }
            
            case OP_ADD: op_add(); break;
            case OP_SUB: op_sub(); break;
            case OP_MUL: op_mul(); break;
            case OP_DIV: op_div(); break;
            case OP_MOD: op_mod(); break;
            case OP_NEG: op_neg(); break;
            
            case OP_AND: op_and(); break;
            case OP_OR: op_or(); break;
            case OP_XOR: op_xor(); break;
            case OP_NOT: op_not(); break;
            case OP_SHL: op_shl(); break;
            case OP_SHR: op_shr(); break;
            
            case OP_EQ: op_eq(); break;
            case OP_NE: op_ne(); break;
            case OP_LT: op_lt(); break;
            case OP_LE: op_le(); break;
            case OP_GT: op_gt(); break;
            case OP_GE: op_ge(); break;
            
            case OP_LAND: op_land(); break;
            case OP_LOR: op_lor(); break;
            case OP_LNOT: op_lnot(); break;
            
            case OP_LOAD_LOCAL: op_load_local(); break;
            case OP_STORE_LOCAL: op_store_local(); break;
            case OP_LOAD_GLOBAL: op_load_global(); break;
            case OP_STORE_GLOBAL: op_store_global(); break;
            
            case OP_JUMP: op_jump(); break;
            case OP_JUMP_IF_TRUE: op_jump_if_true(); break;
            case OP_JUMP_IF_FALSE: op_jump_if_false(); break;
            case OP_CALL: op_call(); break;
            case OP_RETURN: op_return(); break;
            
            case OP_PRINT: op_print(); break;
            case OP_HALT: vm.running = false; break;
            case OP_NOP: break;
            
            default:
                fprintf(stderr, "Unknown opcode: %d\n", instruction);
                exit(1);
        }
    }
}

/* ============================================================================
 * DEBUGGING
 * ============================================================================
 */

void vm_enable_debug() {
    vm.debug_mode = true;
}

void vm_print_stack() {
    printf("\n=== Stack ===\n");
    for (int i = 0; i < vm.sp; i++) {
        printf("[%d] ", i);
        switch (vm.stack[i].type) {
            case VAL_INT: printf("INT: %lld\n", vm.stack[i].as.int_val); break;
            case VAL_FLOAT: printf("FLOAT: %f\n", vm.stack[i].as.float_val); break;
            case VAL_BOOL: printf("BOOL: %s\n", vm.stack[i].as.bool_val ? "true" : "false"); break;
            case VAL_STRING: printf("STRING: %s\n", vm.stack[i].as.string_val); break;
            case VAL_NULL: printf("NULL\n"); break;
            default: printf("OBJECT\n");
        }
    }
    printf("=============\n");
}

void vm_cleanup() {
    /* Free any allocated resources */
    for (int i = 0; i < vm.sp; i++) {
        if (vm.stack[i].type == VAL_STRING && vm.stack[i].as.string_val) {
            free(vm.stack[i].as.string_val);
        }
    }
}

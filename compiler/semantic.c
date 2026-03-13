/* ============================================================================
 * L# Programming Language - Semantic Analyzer Implementation
 * ============================================================================
 * File: semantic.c
 * Purpose: Semantic analysis, symbol resolution, and scope management
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#include "semantic.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * SYMBOL TABLE STRUCTURES
 * ============================================================================
 */

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_CLASS,
    SYMBOL_STRUCT,
    SYMBOL_ENUM,
    SYMBOL_TRAIT,
    SYMBOL_TYPE_ALIAS,
    SYMBOL_MODULE,
    SYMBOL_PARAMETER,
    SYMBOL_FIELD,
    SYMBOL_METHOD,
    SYMBOL_CONSTANT
} SymbolKind;

typedef struct Symbol {
    char *name;
    SymbolKind kind;
    ASTNode *declaration;
    struct Type *type;
    int is_mutable;
    int is_public;
    int scope_level;
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
    int level;
    int is_function_scope;
    int is_loop_scope;
} Scope;

static Scope *current_scope = NULL;
static int scope_counter = 0;

/* ============================================================================
 * SCOPE MANAGEMENT
 * ============================================================================
 */

Scope* create_scope(Scope *parent) {
    Scope *scope = malloc(sizeof(Scope));
    scope->symbols = NULL;
    scope->parent = parent;
    scope->level = parent ? parent->level + 1 : 0;
    scope->is_function_scope = 0;
    scope->is_loop_scope = 0;
    scope_counter++;
    return scope;
}

void enter_scope() {
    current_scope = create_scope(current_scope);
}

void exit_scope() {
    if (current_scope) {
        Scope *old = current_scope;
        current_scope = current_scope->parent;
        
        /* Free symbols in old scope */
        Symbol *sym = old->symbols;
        while (sym) {
            Symbol *next = sym->next;
            free(sym->name);
            free(sym);
            sym = next;
        }
        free(old);
    }
}

void enter_function_scope() {
    enter_scope();
    current_scope->is_function_scope = 1;
}

void enter_loop_scope() {
    enter_scope();
    current_scope->is_loop_scope = 1;
}

int in_loop_scope() {
    Scope *scope = current_scope;
    while (scope) {
        if (scope->is_loop_scope) return 1;
        scope = scope->parent;
    }
    return 0;
}

int in_function_scope() {
    Scope *scope = current_scope;
    while (scope) {
        if (scope->is_function_scope) return 1;
        scope = scope->parent;
    }
    return 0;
}

/* ============================================================================
 * SYMBOL TABLE OPERATIONS
 * ============================================================================
 */

Symbol* create_symbol(const char *name, SymbolKind kind, ASTNode *decl) {
    Symbol *sym = malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->kind = kind;
    sym->declaration = decl;
    sym->type = NULL;
    sym->is_mutable = 0;
    sym->is_public = 0;
    sym->scope_level = current_scope ? current_scope->level : 0;
    sym->next = NULL;
    return sym;
}

void define_symbol(Symbol *sym) {
    if (!current_scope) {
        fprintf(stderr, "Error: No active scope for symbol definition\n");
        return;
    }
    
    /* Check for duplicate in current scope */
    Symbol *existing = current_scope->symbols;
    while (existing) {
        if (strcmp(existing->name, sym->name) == 0) {
            fprintf(stderr, "Error: Symbol '%s' already defined in this scope\n", sym->name);
            return;
        }
        existing = existing->next;
    }
    
    sym->next = current_scope->symbols;
    current_scope->symbols = sym;
}

Symbol* lookup_symbol(const char *name) {
    Scope *scope = current_scope;
    while (scope) {
        Symbol *sym = scope->symbols;
        while (sym) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
            sym = sym->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

Symbol* lookup_symbol_in_scope(const char *name, Scope *scope) {
    Symbol *sym = scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

/* ============================================================================
 * SEMANTIC ANALYSIS - DECLARATIONS
 * ============================================================================
 */

void analyze_variable_declaration(ASTNode *node) {
    if (!node || node->type != AST_VAR_DECL) return;
    
    const char *name = node->data.var_decl.name;
    
    /* Check if already defined in current scope */
    if (lookup_symbol_in_scope(name, current_scope)) {
        fprintf(stderr, "Error: Variable '%s' already defined\n", name);
        return;
    }
    
    /* Analyze initializer */
    if (node->data.var_decl.initializer) {
        analyze_expression(node->data.var_decl.initializer);
    }
    
    /* Create and define symbol */
    Symbol *sym = create_symbol(name, SYMBOL_VARIABLE, node);
    sym->is_mutable = node->data.var_decl.is_mutable;
    define_symbol(sym);
}

void analyze_function_declaration(ASTNode *node) {
    if (!node || node->type != AST_FUNC_DECL) return;
    
    const char *name = node->data.func_decl.name;
    
    /* Define function symbol in current scope */
    Symbol *sym = create_symbol(name, SYMBOL_FUNCTION, node);
    define_symbol(sym);
    
    /* Enter function scope */
    enter_function_scope();
    
    /* Analyze parameters */
    if (node->data.func_decl.parameters) {
        analyze_parameter_list(node->data.func_decl.parameters);
    }
    
    /* Analyze function body */
    if (node->data.func_decl.body) {
        analyze_statement(node->data.func_decl.body);
    }
    
    /* Exit function scope */
    exit_scope();
}

void analyze_class_declaration(ASTNode *node) {
    if (!node || node->type != AST_CLASS_DECL) return;
    
    const char *name = node->data.class_decl.name;
    
    /* Define class symbol */
    Symbol *sym = create_symbol(name, SYMBOL_CLASS, node);
    define_symbol(sym);
    
    /* Enter class scope */
    enter_scope();
    
    /* Analyze class body */
    if (node->data.class_decl.body) {
        analyze_statement(node->data.class_decl.body);
    }
    
    /* Exit class scope */
    exit_scope();
}

void analyze_struct_declaration(ASTNode *node) {
    if (!node || node->type != AST_STRUCT_DECL) return;
    
    const char *name = node->data.struct_decl.name;
    
    /* Define struct symbol */
    Symbol *sym = create_symbol(name, SYMBOL_STRUCT, node);
    define_symbol(sym);
    
    /* Analyze fields */
    if (node->data.struct_decl.fields) {
        analyze_parameter_list(node->data.struct_decl.fields);
    }
}

void analyze_enum_declaration(ASTNode *node) {
    if (!node || node->type != AST_ENUM_DECL) return;
    
    const char *name = node->data.enum_decl.name;
    
    /* Define enum symbol */
    Symbol *sym = create_symbol(name, SYMBOL_ENUM, node);
    define_symbol(sym);
    
    /* Analyze variants */
    if (node->data.enum_decl.variants) {
        analyze_expression(node->data.enum_decl.variants);
    }
}

void analyze_trait_declaration(ASTNode *node) {
    if (!node || node->type != AST_TRAIT_DECL) return;
    
    const char *name = node->data.trait_decl.name;
    
    /* Define trait symbol */
    Symbol *sym = create_symbol(name, SYMBOL_TRAIT, node);
    define_symbol(sym);
    
    /* Enter trait scope */
    enter_scope();
    
    /* Analyze methods */
    if (node->data.trait_decl.methods) {
        analyze_statement(node->data.trait_decl.methods);
    }
    
    /* Exit trait scope */
    exit_scope();
}

void analyze_parameter_list(ASTNode *node) {
    if (!node || node->type != AST_PARAM_LIST) return;
    
    for (int i = 0; i < node->data.param_list.count; i++) {
        ASTNode *param = node->data.param_list.parameters[i];
        if (param && param->type == AST_PARAM) {
            const char *name = param->data.param.name;
            Symbol *sym = create_symbol(name, SYMBOL_PARAMETER, param);
            define_symbol(sym);
            
            /* Analyze default value if present */
            if (param->data.param.default_value) {
                analyze_expression(param->data.param.default_value);
            }
        }
    }
}

/* ============================================================================
 * SEMANTIC ANALYSIS - STATEMENTS
 * ============================================================================
 */

void analyze_statement(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_VAR_DECL:
            analyze_variable_declaration(node);
            break;
            
        case AST_CONST_DECL:
            analyze_variable_declaration(node);
            break;
            
        case AST_FUNC_DECL:
            analyze_function_declaration(node);
            break;
            
        case AST_CLASS_DECL:
            analyze_class_declaration(node);
            break;
            
        case AST_STRUCT_DECL:
            analyze_struct_declaration(node);
            break;
            
        case AST_ENUM_DECL:
            analyze_enum_declaration(node);
            break;
            
        case AST_TRAIT_DECL:
            analyze_trait_declaration(node);
            break;
            
        case AST_IF_STMT:
            analyze_if_statement(node);
            break;
            
        case AST_WHILE_STMT:
            analyze_while_statement(node);
            break;
            
        case AST_FOR_STMT:
            analyze_for_statement(node);
            break;
            
        case AST_MATCH_STMT:
            analyze_match_statement(node);
            break;
            
        case AST_RETURN_STMT:
            analyze_return_statement(node);
            break;
            
        case AST_BREAK_STMT:
            analyze_break_statement(node);
            break;
            
        case AST_CONTINUE_STMT:
            analyze_continue_statement(node);
            break;
            
        case AST_EXPR_STMT:
            if (node->data.expr_stmt.expression) {
                analyze_expression(node->data.expr_stmt.expression);
            }
            break;
            
        case AST_BLOCK:
            analyze_block(node);
            break;
            
        case AST_STATEMENT_LIST:
            for (int i = 0; i < node->data.statement_list.count; i++) {
                analyze_statement(node->data.statement_list.statements[i]);
            }
            break;
            
        default:
            break;
    }
}

void analyze_if_statement(ASTNode *node) {
    if (!node || node->type != AST_IF_STMT) return;
    
    /* Analyze condition */
    analyze_expression(node->data.if_stmt.condition);
    
    /* Analyze then branch */
    if (node->data.if_stmt.then_branch) {
        analyze_statement(node->data.if_stmt.then_branch);
    }
    
    /* Analyze else branch */
    if (node->data.if_stmt.else_branch) {
        analyze_statement(node->data.if_stmt.else_branch);
    }
}

void analyze_while_statement(ASTNode *node) {
    if (!node || node->type != AST_WHILE_STMT) return;
    
    /* Analyze condition */
    analyze_expression(node->data.while_stmt.condition);
    
    /* Enter loop scope */
    enter_loop_scope();
    
    /* Analyze body */
    if (node->data.while_stmt.body) {
        analyze_statement(node->data.while_stmt.body);
    }
    
    /* Exit loop scope */
    exit_scope();
}

void analyze_for_statement(ASTNode *node) {
    if (!node || node->type != AST_FOR_STMT) return;
    
    /* Enter loop scope */
    enter_loop_scope();
    
    /* Define iterator variable */
    Symbol *sym = create_symbol(node->data.for_stmt.iterator, SYMBOL_VARIABLE, node);
    define_symbol(sym);
    
    /* Analyze iterable */
    analyze_expression(node->data.for_stmt.iterable);
    
    /* Analyze body */
    if (node->data.for_stmt.body) {
        analyze_statement(node->data.for_stmt.body);
    }
    
    /* Exit loop scope */
    exit_scope();
}

void analyze_match_statement(ASTNode *node) {
    if (!node || node->type != AST_MATCH_STMT) return;
    
    /* Analyze expression */
    analyze_expression(node->data.match_stmt.expression);
    
    /* Analyze match arms */
    if (node->data.match_stmt.arms) {
        analyze_match_arms(node->data.match_stmt.arms);
    }
}

void analyze_match_arms(ASTNode *node) {
    if (!node || node->type != AST_MATCH_ARM_LIST) return;
    
    for (int i = 0; i < node->data.match_arm_list.count; i++) {
        ASTNode *arm = node->data.match_arm_list.arms[i];
        if (arm && arm->type == AST_MATCH_ARM) {
            /* Enter scope for pattern bindings */
            enter_scope();
            
            /* Analyze pattern */
            analyze_pattern(arm->data.match_arm.pattern);
            
            /* Analyze guard if present */
            if (arm->data.match_arm.guard) {
                analyze_expression(arm->data.match_arm.guard);
            }
            
            /* Analyze body */
            analyze_statement(arm->data.match_arm.body);
            
            /* Exit scope */
            exit_scope();
        }
    }
}

void analyze_return_statement(ASTNode *node) {
    if (!node || node->type != AST_RETURN_STMT) return;
    
    /* Check if in function scope */
    if (!in_function_scope()) {
        fprintf(stderr, "Error: Return statement outside function\n");
        return;
    }
    
    /* Analyze return value */
    if (node->data.return_stmt.value) {
        analyze_expression(node->data.return_stmt.value);
    }
}

void analyze_break_statement(ASTNode *node) {
    if (!node || node->type != AST_BREAK_STMT) return;
    
    /* Check if in loop scope */
    if (!in_loop_scope()) {
        fprintf(stderr, "Error: Break statement outside loop\n");
        return;
    }
}

void analyze_continue_statement(ASTNode *node) {
    if (!node || node->type != AST_CONTINUE_STMT) return;
    
    /* Check if in loop scope */
    if (!in_loop_scope()) {
        fprintf(stderr, "Error: Continue statement outside loop\n");
        return;
    }
}

void analyze_block(ASTNode *node) {
    if (!node || node->type != AST_BLOCK) return;
    
    /* Enter block scope */
    enter_scope();
    
    /* Analyze statements */
    if (node->data.block.statements) {
        analyze_statement(node->data.block.statements);
    }
    
    /* Exit block scope */
    exit_scope();
}

/* ============================================================================
 * SEMANTIC ANALYSIS - EXPRESSIONS
 * ============================================================================
 */

void analyze_expression(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_INT_LITERAL:
        case AST_FLOAT_LITERAL:
        case AST_STRING_LITERAL:
        case AST_BOOL_LITERAL:
        case AST_NULL_LITERAL:
        case AST_CHAR_LITERAL:
            /* Literals need no analysis */
            break;
            
        case AST_IDENTIFIER:
            analyze_identifier(node);
            break;
            
        case AST_BINARY_EXPR:
            analyze_binary_expression(node);
            break;
            
        case AST_UNARY_EXPR:
            analyze_unary_expression(node);
            break;
            
        case AST_CALL_EXPR:
            analyze_call_expression(node);
            break;
            
        case AST_MEMBER_EXPR:
            analyze_member_expression(node);
            break;
            
        case AST_INDEX_EXPR:
            analyze_index_expression(node);
            break;
            
        case AST_LAMBDA_EXPR:
            analyze_lambda_expression(node);
            break;
            
        case AST_ARRAY_LITERAL:
            analyze_array_literal(node);
            break;
            
        case AST_STRUCT_LITERAL:
            analyze_struct_literal(node);
            break;
            
        case AST_EXPR_LIST:
            for (int i = 0; i < node->data.expr_list.count; i++) {
                analyze_expression(node->data.expr_list.expressions[i]);
            }
            break;
            
        default:
            break;
    }
}

void analyze_identifier(ASTNode *node) {
    if (!node || node->type != AST_IDENTIFIER) return;
    
    const char *name = node->data.identifier.name;
    Symbol *sym = lookup_symbol(name);
    
    if (!sym) {
        fprintf(stderr, "Error: Undefined identifier '%s'\n", name);
        return;
    }
    
    /* Store resolved symbol */
    node->data.identifier.resolved_symbol = sym;
}

void analyze_binary_expression(ASTNode *node) {
    if (!node || node->type != AST_BINARY_EXPR) return;
    
    /* Analyze operands */
    analyze_expression(node->data.binary_expr.left);
    analyze_expression(node->data.binary_expr.right);
    
    /* Check operator validity */
    const char *op = node->data.binary_expr.operator;
    
    /* Assignment operators require lvalue */
    if (strcmp(op, "=") == 0 || strcmp(op, "+=") == 0 || 
        strcmp(op, "-=") == 0 || strcmp(op, "*=") == 0) {
        if (!is_lvalue(node->data.binary_expr.left)) {
            fprintf(stderr, "Error: Left side of assignment must be an lvalue\n");
        }
    }
}

void analyze_unary_expression(ASTNode *node) {
    if (!node || node->type != AST_UNARY_EXPR) return;
    
    /* Analyze operand */
    analyze_expression(node->data.unary_expr.operand);
}

void analyze_call_expression(ASTNode *node) {
    if (!node || node->type != AST_CALL_EXPR) return;
    
    /* Analyze callee */
    analyze_expression(node->data.call_expr.callee);
    
    /* Analyze arguments */
    if (node->data.call_expr.arguments) {
        analyze_expression(node->data.call_expr.arguments);
    }
}

void analyze_member_expression(ASTNode *node) {
    if (!node || node->type != AST_MEMBER_EXPR) return;
    
    /* Analyze object */
    analyze_expression(node->data.member_expr.object);
}

void analyze_index_expression(ASTNode *node) {
    if (!node || node->type != AST_INDEX_EXPR) return;
    
    /* Analyze array */
    analyze_expression(node->data.index_expr.array);
    
    /* Analyze index */
    analyze_expression(node->data.index_expr.index);
}

void analyze_lambda_expression(ASTNode *node) {
    if (!node || node->type != AST_LAMBDA_EXPR) return;
    
    /* Enter lambda scope */
    enter_function_scope();
    
    /* Analyze parameters */
    if (node->data.lambda_expr.parameters) {
        analyze_parameter_list(node->data.lambda_expr.parameters);
    }
    
    /* Analyze body */
    if (node->data.lambda_expr.body) {
        if (node->data.lambda_expr.has_block_body) {
            analyze_statement(node->data.lambda_expr.body);
        } else {
            analyze_expression(node->data.lambda_expr.body);
        }
    }
    
    /* Exit lambda scope */
    exit_scope();
}

void analyze_array_literal(ASTNode *node) {
    if (!node || node->type != AST_ARRAY_LITERAL) return;
    
    /* Analyze elements */
    if (node->data.array_literal.elements) {
        analyze_expression(node->data.array_literal.elements);
    }
}

void analyze_struct_literal(ASTNode *node) {
    if (!node || node->type != AST_STRUCT_LITERAL) return;
    
    /* Look up struct type */
    Symbol *sym = lookup_symbol(node->data.struct_literal.name);
    if (!sym || sym->kind != SYMBOL_STRUCT) {
        fprintf(stderr, "Error: Undefined struct '%s'\n", node->data.struct_literal.name);
        return;
    }
    
    /* Analyze field values */
    if (node->data.struct_literal.fields) {
        analyze_expression(node->data.struct_literal.fields);
    }
}

/* ============================================================================
 * SEMANTIC ANALYSIS - PATTERNS
 * ============================================================================
 */

void analyze_pattern(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_IDENTIFIER:
            /* Pattern binding - define new variable */
            {
                const char *name = node->data.identifier.name;
                Symbol *sym = create_symbol(name, SYMBOL_VARIABLE, node);
                define_symbol(sym);
            }
            break;
            
        case AST_STRUCT_PATTERN:
            analyze_struct_pattern(node);
            break;
            
        case AST_ENUM_PATTERN:
            analyze_enum_pattern(node);
            break;
            
        case AST_OR_PATTERN:
            analyze_pattern(node->data.or_pattern.left);
            analyze_pattern(node->data.or_pattern.right);
            break;
            
        case AST_BINDING_PATTERN:
            {
                const char *name = node->data.binding_pattern.name;
                Symbol *sym = create_symbol(name, SYMBOL_VARIABLE, node);
                define_symbol(sym);
                analyze_pattern(node->data.binding_pattern.pattern);
            }
            break;
            
        default:
            break;
    }
}

void analyze_struct_pattern(ASTNode *node) {
    if (!node || node->type != AST_STRUCT_PATTERN) return;
    
    /* Look up struct type */
    Symbol *sym = lookup_symbol(node->data.struct_pattern.name);
    if (!sym || sym->kind != SYMBOL_STRUCT) {
        fprintf(stderr, "Error: Undefined struct '%s' in pattern\n", 
                node->data.struct_pattern.name);
        return;
    }
    
    /* Analyze field patterns */
    if (node->data.struct_pattern.fields) {
        analyze_pattern(node->data.struct_pattern.fields);
    }
}

void analyze_enum_pattern(ASTNode *node) {
    if (!node || node->type != AST_ENUM_PATTERN) return;
    
    /* Look up enum type */
    Symbol *sym = lookup_symbol(node->data.enum_pattern.enum_name);
    if (!sym || sym->kind != SYMBOL_ENUM) {
        fprintf(stderr, "Error: Undefined enum '%s' in pattern\n", 
                node->data.enum_pattern.enum_name);
        return;
    }
    
    /* Analyze field patterns */
    if (node->data.enum_pattern.fields) {
        analyze_pattern(node->data.enum_pattern.fields);
    }
}

/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================
 */

int is_lvalue(ASTNode *node) {
    if (!node) return 0;
    
    switch (node->type) {
        case AST_IDENTIFIER:
            return 1;
        case AST_MEMBER_EXPR:
            return 1;
        case AST_INDEX_EXPR:
            return 1;
        case AST_UNARY_EXPR:
            if (strcmp(node->data.unary_expr.operator, "*") == 0) {
                return 1; /* Dereference is lvalue */
            }
            return 0;
        default:
            return 0;
    }
}

/* ============================================================================
 * MAIN SEMANTIC ANALYSIS ENTRY POINT
 * ============================================================================
 */

void analyze_program(ASTNode *root) {
    /* Initialize global scope */
    enter_scope();
    
    /* Analyze the program */
    analyze_statement(root);
    
    /* Exit global scope */
    exit_scope();
}

/* ============================================================================
 * DEBUGGING AND REPORTING
 * ============================================================================
 */

void print_symbol_table() {
    printf("\n=== Symbol Table ===\n");
    Scope *scope = current_scope;
    int level = 0;
    
    while (scope) {
        printf("Scope Level %d:\n", level);
        Symbol *sym = scope->symbols;
        while (sym) {
            printf("  %s: ", sym->name);
            switch (sym->kind) {
                case SYMBOL_VARIABLE: printf("Variable"); break;
                case SYMBOL_FUNCTION: printf("Function"); break;
                case SYMBOL_CLASS: printf("Class"); break;
                case SYMBOL_STRUCT: printf("Struct"); break;
                case SYMBOL_ENUM: printf("Enum"); break;
                case SYMBOL_TRAIT: printf("Trait"); break;
                default: printf("Unknown");
            }
            printf("\n");
            sym = sym->next;
        }
        scope = scope->parent;
        level++;
    }
    printf("====================\n");
}

void print_semantic_statistics() {
    printf("\n=== Semantic Analysis Statistics ===\n");
    printf("Total scopes created: %d\n", scope_counter);
    printf("====================================\n");
}

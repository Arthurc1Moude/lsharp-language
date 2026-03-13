/* ============================================================================
 * L# Programming Language - Abstract Syntax Tree Implementation
 * ============================================================================
 * File: ast.c
 * Purpose: Complete AST node creation and manipulation functions
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * MEMORY MANAGEMENT
 * ============================================================================
 */

static size_t total_nodes_allocated = 0;
static size_t total_memory_used = 0;

ASTNode* allocate_node(ASTNodeType type) {
    ASTNode *node = (ASTNode*)calloc(1, sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Fatal: Out of memory allocating AST node\n");
        exit(1);
    }
    node->type = type;
    node->line = 0;
    node->column = 0;
    total_nodes_allocated++;
    total_memory_used += sizeof(ASTNode);
    return node;
}

void free_ast_node(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_STRING_LITERAL:
            if (node->data.string_literal.value) {
                free(node->data.string_literal.value);
            }
            break;
        case AST_IDENTIFIER:
            if (node->data.identifier.name) {
                free(node->data.identifier.name);
            }
            break;
        default:
            break;
    }
    
    free(node);
    total_nodes_allocated--;
}

/* ============================================================================
 * LITERAL NODE CREATION
 * ============================================================================
 */

ASTNode* create_int_literal(long long value) {
    ASTNode *node = allocate_node(AST_INT_LITERAL);
    node->data.int_literal.value = value;
    node->data.int_literal.suffix = NULL;
    return node;
}

ASTNode* create_float_literal(double value) {
    ASTNode *node = allocate_node(AST_FLOAT_LITERAL);
    node->data.float_literal.value = value;
    node->data.float_literal.suffix = NULL;
    return node;
}

ASTNode* create_string_literal(const char *value) {
    ASTNode *node = allocate_node(AST_STRING_LITERAL);
    node->data.string_literal.value = strdup(value);
    node->data.string_literal.length = strlen(value);
    node->data.string_literal.is_raw = 0;
    return node;
}

ASTNode* create_bool_literal(int value) {
    ASTNode *node = allocate_node(AST_BOOL_LITERAL);
    node->data.bool_literal.value = value;
    return node;
}

ASTNode* create_null_literal() {
    ASTNode *node = allocate_node(AST_NULL_LITERAL);
    return node;
}

ASTNode* create_char_literal(char value) {
    ASTNode *node = allocate_node(AST_CHAR_LITERAL);
    node->data.char_literal.value = value;
    return node;
}

/* ============================================================================
 * IDENTIFIER AND TYPE NODES
 * ============================================================================
 */

ASTNode* create_identifier(const char *name) {
    ASTNode *node = allocate_node(AST_IDENTIFIER);
    node->data.identifier.name = strdup(name);
    node->data.identifier.resolved_symbol = NULL;
    return node;
}

ASTNode* create_type_node(const char *name) {
    ASTNode *node = allocate_node(AST_TYPE);
    node->data.type_node.name = strdup(name);
    node->data.type_node.params = NULL;
    node->data.type_node.param_count = 0;
    return node;
}

ASTNode* create_primitive_type(const char *name) {
    ASTNode *node = allocate_node(AST_PRIMITIVE_TYPE);
    node->data.primitive_type.name = strdup(name);
    return node;
}

ASTNode* create_array_type(ASTNode *element_type) {
    ASTNode *node = allocate_node(AST_ARRAY_TYPE);
    node->data.array_type.element_type = element_type;
    node->data.array_type.size = NULL;
    return node;
}

ASTNode* create_generic_type(const char *name, ASTNode *type_arg) {
    ASTNode *node = allocate_node(AST_GENERIC_TYPE);
    node->data.generic_type.name = strdup(name);
    node->data.generic_type.type_args = malloc(sizeof(ASTNode*));
    node->data.generic_type.type_args[0] = type_arg;
    node->data.generic_type.arg_count = 1;
    return node;
}

/* ============================================================================
 * EXPRESSION NODES
 * ============================================================================
 */

ASTNode* create_binary_expr(const char *op, ASTNode *left, ASTNode *right) {
    ASTNode *node = allocate_node(AST_BINARY_EXPR);
    node->data.binary_expr.operator = strdup(op);
    node->data.binary_expr.left = left;
    node->data.binary_expr.right = right;
    node->data.binary_expr.result_type = NULL;
    return node;
}

ASTNode* create_unary_expr(const char *op, ASTNode *operand) {
    ASTNode *node = allocate_node(AST_UNARY_EXPR);
    node->data.unary_expr.operator = strdup(op);
    node->data.unary_expr.operand = operand;
    node->data.unary_expr.is_prefix = 1;
    return node;
}

ASTNode* create_call_expr(ASTNode *callee, ASTNode *args) {
    ASTNode *node = allocate_node(AST_CALL_EXPR);
    node->data.call_expr.callee = callee;
    node->data.call_expr.arguments = args;
    node->data.call_expr.arg_count = 0;
    return node;
}

ASTNode* create_member_expr(ASTNode *object, const char *member) {
    ASTNode *node = allocate_node(AST_MEMBER_EXPR);
    node->data.member_expr.object = object;
    node->data.member_expr.member = strdup(member);
    node->data.member_expr.is_computed = 0;
    return node;
}

ASTNode* create_index_expr(ASTNode *array, ASTNode *index) {
    ASTNode *node = allocate_node(AST_INDEX_EXPR);
    node->data.index_expr.array = array;
    node->data.index_expr.index = index;
    return node;
}

ASTNode* create_ternary_expr(ASTNode *condition, ASTNode *then_expr, ASTNode *else_expr) {
    ASTNode *node = allocate_node(AST_TERNARY_EXPR);
    node->data.ternary_expr.condition = condition;
    node->data.ternary_expr.then_expr = then_expr;
    node->data.ternary_expr.else_expr = else_expr;
    return node;
}

ASTNode* create_cast_expr(ASTNode *expr, ASTNode *target_type) {
    ASTNode *node = allocate_node(AST_CAST_EXPR);
    node->data.cast_expr.expression = expr;
    node->data.cast_expr.target_type = target_type;
    node->data.cast_expr.is_safe = 1;
    return node;
}

ASTNode* create_lambda_expr(ASTNode *params, ASTNode *body, int has_block) {
    ASTNode *node = allocate_node(AST_LAMBDA_EXPR);
    node->data.lambda_expr.parameters = params;
    node->data.lambda_expr.body = body;
    node->data.lambda_expr.has_block_body = has_block;
    node->data.lambda_expr.captures = NULL;
    return node;
}

/* ============================================================================
 * STATEMENT NODES
 * ============================================================================
 */

ASTNode* create_var_decl(const char *name, ASTNode *type, ASTNode *init, int is_mutable) {
    ASTNode *node = allocate_node(AST_VAR_DECL);
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.type = type;
    node->data.var_decl.initializer = init;
    node->data.var_decl.is_mutable = is_mutable;
    return node;
}

ASTNode* create_const_decl(const char *name, ASTNode *init) {
    ASTNode *node = allocate_node(AST_CONST_DECL);
    node->data.const_decl.name = strdup(name);
    node->data.const_decl.initializer = init;
    node->data.const_decl.type = NULL;
    return node;
}

ASTNode* create_func_decl(const char *name, ASTNode *params, ASTNode *return_type, 
                          ASTNode *body, int is_async) {
    ASTNode *node = allocate_node(AST_FUNC_DECL);
    node->data.func_decl.name = strdup(name);
    node->data.func_decl.parameters = params;
    node->data.func_decl.return_type = return_type;
    node->data.func_decl.body = body;
    node->data.func_decl.is_async = is_async;
    node->data.func_decl.is_generic = 0;
    return node;
}

ASTNode* create_class_decl(const char *name, ASTNode *superclass, ASTNode *body) {
    ASTNode *node = allocate_node(AST_CLASS_DECL);
    node->data.class_decl.name = strdup(name);
    node->data.class_decl.superclass = superclass;
    node->data.class_decl.body = body;
    node->data.class_decl.interfaces = NULL;
    return node;
}

ASTNode* create_struct_decl(const char *name, ASTNode *fields) {
    ASTNode *node = allocate_node(AST_STRUCT_DECL);
    node->data.struct_decl.name = strdup(name);
    node->data.struct_decl.fields = fields;
    node->data.struct_decl.is_packed = 0;
    return node;
}

ASTNode* create_enum_decl(const char *name, ASTNode *variants) {
    ASTNode *node = allocate_node(AST_ENUM_DECL);
    node->data.enum_decl.name = strdup(name);
    node->data.enum_decl.variants = variants;
    node->data.enum_decl.underlying_type = NULL;
    return node;
}

ASTNode* create_trait_decl(const char *name, ASTNode *methods) {
    ASTNode *node = allocate_node(AST_TRAIT_DECL);
    node->data.trait_decl.name = strdup(name);
    node->data.trait_decl.methods = methods;
    node->data.trait_decl.supertraits = NULL;
    return node;
}

/* ============================================================================
 * CONTROL FLOW NODES
 * ============================================================================
 */

ASTNode* create_if_stmt(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = allocate_node(AST_IF_STMT);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

ASTNode* create_while_stmt(ASTNode *condition, ASTNode *body) {
    ASTNode *node = allocate_node(AST_WHILE_STMT);
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    node->data.while_stmt.label = NULL;
    return node;
}

ASTNode* create_for_stmt(const char *iterator, ASTNode *iterable, ASTNode *body) {
    ASTNode *node = allocate_node(AST_FOR_STMT);
    node->data.for_stmt.iterator = strdup(iterator);
    node->data.for_stmt.iterable = iterable;
    node->data.for_stmt.body = body;
    node->data.for_stmt.label = NULL;
    return node;
}

ASTNode* create_match_stmt(ASTNode *expr, ASTNode *arms) {
    ASTNode *node = allocate_node(AST_MATCH_STMT);
    node->data.match_stmt.expression = expr;
    node->data.match_stmt.arms = arms;
    node->data.match_stmt.is_exhaustive = 0;
    return node;
}

ASTNode* create_return_stmt(ASTNode *value) {
    ASTNode *node = allocate_node(AST_RETURN_STMT);
    node->data.return_stmt.value = value;
    return node;
}

ASTNode* create_break_stmt() {
    ASTNode *node = allocate_node(AST_BREAK_STMT);
    node->data.break_stmt.label = NULL;
    node->data.break_stmt.value = NULL;
    return node;
}

ASTNode* create_continue_stmt() {
    ASTNode *node = allocate_node(AST_CONTINUE_STMT);
    node->data.continue_stmt.label = NULL;
    return node;
}

ASTNode* create_expr_stmt(ASTNode *expr) {
    ASTNode *node = allocate_node(AST_EXPR_STMT);
    node->data.expr_stmt.expression = expr;
    return node;
}

ASTNode* create_block(ASTNode *statements) {
    ASTNode *node = allocate_node(AST_BLOCK);
    node->data.block.statements = statements;
    node->data.block.statement_count = 0;
    node->data.block.scope_id = 0;
    return node;
}

/* ============================================================================
 * PATTERN MATCHING NODES
 * ============================================================================
 */

ASTNode* create_match_arm(ASTNode *pattern, ASTNode *body) {
    ASTNode *node = allocate_node(AST_MATCH_ARM);
    node->data.match_arm.pattern = pattern;
    node->data.match_arm.guard = NULL;
    node->data.match_arm.body = body;
    return node;
}

ASTNode* create_range_pattern(ASTNode *start, ASTNode *end, int inclusive) {
    ASTNode *node = allocate_node(AST_RANGE_PATTERN);
    node->data.range_pattern.start = start;
    node->data.range_pattern.end = end;
    node->data.range_pattern.is_inclusive = inclusive;
    return node;
}

ASTNode* create_struct_pattern(const char *name, ASTNode *fields) {
    ASTNode *node = allocate_node(AST_STRUCT_PATTERN);
    node->data.struct_pattern.name = strdup(name);
    node->data.struct_pattern.fields = fields;
    node->data.struct_pattern.has_rest = 0;
    return node;
}

ASTNode* create_enum_pattern(const char *enum_name, const char *variant, ASTNode *fields) {
    ASTNode *node = allocate_node(AST_ENUM_PATTERN);
    node->data.enum_pattern.enum_name = strdup(enum_name);
    node->data.enum_pattern.variant = strdup(variant);
    node->data.enum_pattern.fields = fields;
    return node;
}

ASTNode* create_or_pattern(ASTNode *left, ASTNode *right) {
    ASTNode *node = allocate_node(AST_OR_PATTERN);
    node->data.or_pattern.left = left;
    node->data.or_pattern.right = right;
    return node;
}

ASTNode* create_binding_pattern(const char *name, ASTNode *pattern) {
    ASTNode *node = allocate_node(AST_BINDING_PATTERN);
    node->data.binding_pattern.name = strdup(name);
    node->data.binding_pattern.pattern = pattern;
    return node;
}

/* ============================================================================
 * LIST CONSTRUCTION HELPERS
 * ============================================================================
 */

ASTNode* create_statement_list(ASTNode *stmt) {
    ASTNode *node = allocate_node(AST_STATEMENT_LIST);
    node->data.statement_list.statements = malloc(sizeof(ASTNode*) * 16);
    node->data.statement_list.statements[0] = stmt;
    node->data.statement_list.count = 1;
    node->data.statement_list.capacity = 16;
    return node;
}

ASTNode* append_statement(ASTNode *list, ASTNode *stmt) {
    if (list->data.statement_list.count >= list->data.statement_list.capacity) {
        list->data.statement_list.capacity *= 2;
        list->data.statement_list.statements = realloc(
            list->data.statement_list.statements,
            sizeof(ASTNode*) * list->data.statement_list.capacity
        );
    }
    list->data.statement_list.statements[list->data.statement_list.count++] = stmt;
    return list;
}

ASTNode* create_param_list(ASTNode *param) {
    ASTNode *node = allocate_node(AST_PARAM_LIST);
    node->data.param_list.parameters = malloc(sizeof(ASTNode*) * 8);
    node->data.param_list.parameters[0] = param;
    node->data.param_list.count = 1;
    node->data.param_list.capacity = 8;
    return node;
}

ASTNode* append_param(ASTNode *list, ASTNode *param) {
    if (list->data.param_list.count >= list->data.param_list.capacity) {
        list->data.param_list.capacity *= 2;
        list->data.param_list.parameters = realloc(
            list->data.param_list.parameters,
            sizeof(ASTNode*) * list->data.param_list.capacity
        );
    }
    list->data.param_list.parameters[list->data.param_list.count++] = param;
    return list;
}

ASTNode* create_param(const char *name, ASTNode *type, ASTNode *default_value) {
    ASTNode *node = allocate_node(AST_PARAM);
    node->data.param.name = strdup(name);
    node->data.param.type = type;
    node->data.param.default_value = default_value;
    node->data.param.is_variadic = 0;
    return node;
}

ASTNode* create_expr_list(ASTNode *expr) {
    ASTNode *node = allocate_node(AST_EXPR_LIST);
    node->data.expr_list.expressions = malloc(sizeof(ASTNode*) * 8);
    node->data.expr_list.expressions[0] = expr;
    node->data.expr_list.count = 1;
    node->data.expr_list.capacity = 8;
    return node;
}

ASTNode* append_expr(ASTNode *list, ASTNode *expr) {
    if (list->data.expr_list.count >= list->data.expr_list.capacity) {
        list->data.expr_list.capacity *= 2;
        list->data.expr_list.expressions = realloc(
            list->data.expr_list.expressions,
            sizeof(ASTNode*) * list->data.expr_list.capacity
        );
    }
    list->data.expr_list.expressions[list->data.expr_list.count++] = expr;
    return list;
}

ASTNode* create_arg_list(ASTNode *arg) {
    return create_expr_list(arg);
}

ASTNode* append_arg(ASTNode *list, ASTNode *arg) {
    return append_expr(list, arg);
}

ASTNode* create_match_arm_list(ASTNode *arm) {
    ASTNode *node = allocate_node(AST_MATCH_ARM_LIST);
    node->data.match_arm_list.arms = malloc(sizeof(ASTNode*) * 8);
    node->data.match_arm_list.arms[0] = arm;
    node->data.match_arm_list.count = 1;
    node->data.match_arm_list.capacity = 8;
    return node;
}

ASTNode* append_match_arm(ASTNode *list, ASTNode *arm) {
    if (list->data.match_arm_list.count >= list->data.match_arm_list.capacity) {
        list->data.match_arm_list.capacity *= 2;
        list->data.match_arm_list.arms = realloc(
            list->data.match_arm_list.arms,
            sizeof(ASTNode*) * list->data.match_arm_list.capacity
        );
    }
    list->data.match_arm_list.arms[list->data.match_arm_list.count++] = arm;
    return list;
}

/* ============================================================================
 * ARRAY AND STRUCT LITERALS
 * ============================================================================
 */

ASTNode* create_array_literal(ASTNode *elements) {
    ASTNode *node = allocate_node(AST_ARRAY_LITERAL);
    node->data.array_literal.elements = elements;
    node->data.array_literal.element_count = 0;
    return node;
}

ASTNode* create_struct_literal(const char *name, ASTNode *fields) {
    ASTNode *node = allocate_node(AST_STRUCT_LITERAL);
    node->data.struct_literal.name = strdup(name);
    node->data.struct_literal.fields = fields;
    node->data.struct_literal.field_count = 0;
    return node;
}

/* ============================================================================
 * AST TRAVERSAL AND VISITORS
 * ============================================================================
 */

typedef void (*ASTVisitorFunc)(ASTNode *node, void *context);

void traverse_ast_preorder(ASTNode *node, ASTVisitorFunc visitor, void *context) {
    if (!node) return;
    
    visitor(node, context);
    
    switch (node->type) {
        case AST_BINARY_EXPR:
            traverse_ast_preorder(node->data.binary_expr.left, visitor, context);
            traverse_ast_preorder(node->data.binary_expr.right, visitor, context);
            break;
        case AST_UNARY_EXPR:
            traverse_ast_preorder(node->data.unary_expr.operand, visitor, context);
            break;
        case AST_IF_STMT:
            traverse_ast_preorder(node->data.if_stmt.condition, visitor, context);
            traverse_ast_preorder(node->data.if_stmt.then_branch, visitor, context);
            traverse_ast_preorder(node->data.if_stmt.else_branch, visitor, context);
            break;
        default:
            break;
    }
}

void traverse_ast_postorder(ASTNode *node, ASTVisitorFunc visitor, void *context) {
    if (!node) return;
    
    switch (node->type) {
        case AST_BINARY_EXPR:
            traverse_ast_postorder(node->data.binary_expr.left, visitor, context);
            traverse_ast_postorder(node->data.binary_expr.right, visitor, context);
            break;
        case AST_UNARY_EXPR:
            traverse_ast_postorder(node->data.unary_expr.operand, visitor, context);
            break;
        case AST_IF_STMT:
            traverse_ast_postorder(node->data.if_stmt.condition, visitor, context);
            traverse_ast_postorder(node->data.if_stmt.then_branch, visitor, context);
            traverse_ast_postorder(node->data.if_stmt.else_branch, visitor, context);
            break;
        default:
            break;
    }
    
    visitor(node, context);
}

/* ============================================================================
 * AST CLONING
 * ============================================================================
 */

ASTNode* clone_ast_node(ASTNode *node) {
    if (!node) return NULL;
    
    ASTNode *clone = allocate_node(node->type);
    clone->line = node->line;
    clone->column = node->column;
    
    switch (node->type) {
        case AST_INT_LITERAL:
            clone->data.int_literal = node->data.int_literal;
            break;
        case AST_FLOAT_LITERAL:
            clone->data.float_literal = node->data.float_literal;
            break;
        case AST_STRING_LITERAL:
            clone->data.string_literal.value = strdup(node->data.string_literal.value);
            clone->data.string_literal.length = node->data.string_literal.length;
            break;
        case AST_BINARY_EXPR:
            clone->data.binary_expr.operator = strdup(node->data.binary_expr.operator);
            clone->data.binary_expr.left = clone_ast_node(node->data.binary_expr.left);
            clone->data.binary_expr.right = clone_ast_node(node->data.binary_expr.right);
            break;
        default:
            break;
    }
    
    return clone;
}

/* ============================================================================
 * AST PRINTING AND DEBUGGING
 * ============================================================================
 */

void print_ast_node(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_INT_LITERAL:
            printf("IntLiteral(%lld)\n", node->data.int_literal.value);
            break;
        case AST_FLOAT_LITERAL:
            printf("FloatLiteral(%f)\n", node->data.float_literal.value);
            break;
        case AST_STRING_LITERAL:
            printf("StringLiteral(\"%s\")\n", node->data.string_literal.value);
            break;
        case AST_IDENTIFIER:
            printf("Identifier(%s)\n", node->data.identifier.name);
            break;
        case AST_BINARY_EXPR:
            printf("BinaryExpr(%s)\n", node->data.binary_expr.operator);
            print_ast_node(node->data.binary_expr.left, indent + 1);
            print_ast_node(node->data.binary_expr.right, indent + 1);
            break;
        case AST_UNARY_EXPR:
            printf("UnaryExpr(%s)\n", node->data.unary_expr.operator);
            print_ast_node(node->data.unary_expr.operand, indent + 1);
            break;
        default:
            printf("Node(type=%d)\n", node->type);
            break;
    }
}

void print_ast_json(ASTNode *node, FILE *output, int indent) {
    if (!node) {
        fprintf(output, "null");
        return;
    }
    
    fprintf(output, "{\n");
    for (int i = 0; i < indent + 1; i++) fprintf(output, "  ");
    fprintf(output, "\"type\": %d,\n", node->type);
    
    for (int i = 0; i < indent + 1; i++) fprintf(output, "  ");
    fprintf(output, "\"line\": %d,\n", node->line);
    
    for (int i = 0; i < indent + 1; i++) fprintf(output, "  ");
    fprintf(output, "\"column\": %d\n", node->column);
    
    for (int i = 0; i < indent; i++) fprintf(output, "  ");
    fprintf(output, "}");
}

/* ============================================================================
 * AST STATISTICS
 * ============================================================================
 */

void print_ast_statistics() {
    printf("\n=== AST Statistics ===\n");
    printf("Total nodes allocated: %zu\n", total_nodes_allocated);
    printf("Total memory used: %zu bytes\n", total_memory_used);
    printf("======================\n");
}

size_t count_ast_nodes(ASTNode *node) {
    if (!node) return 0;
    
    size_t count = 1;
    
    switch (node->type) {
        case AST_BINARY_EXPR:
            count += count_ast_nodes(node->data.binary_expr.left);
            count += count_ast_nodes(node->data.binary_expr.right);
            break;
        case AST_UNARY_EXPR:
            count += count_ast_nodes(node->data.unary_expr.operand);
            break;
        default:
            break;
    }
    
    return count;
}

int get_ast_depth(ASTNode *node) {
    if (!node) return 0;
    
    int max_depth = 0;
    
    switch (node->type) {
        case AST_BINARY_EXPR: {
            int left_depth = get_ast_depth(node->data.binary_expr.left);
            int right_depth = get_ast_depth(node->data.binary_expr.right);
            max_depth = (left_depth > right_depth) ? left_depth : right_depth;
            break;
        }
        case AST_UNARY_EXPR:
            max_depth = get_ast_depth(node->data.unary_expr.operand);
            break;
        default:
            break;
    }
    
    return max_depth + 1;
}

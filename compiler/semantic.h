/* ============================================================================
 * L# Programming Language - Semantic Analyzer Header
 * ============================================================================
 * File: semantic.h
 * Purpose: Semantic analysis function declarations
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#ifndef LSHARP_SEMANTIC_H
#define LSHARP_SEMANTIC_H

#include "ast.h"

/* Scope Management */
void enter_scope(void);
void exit_scope(void);
void enter_function_scope(void);
void enter_loop_scope(void);
int in_loop_scope(void);
int in_function_scope(void);

/* Declaration Analysis */
void analyze_variable_declaration(ASTNode *node);
void analyze_function_declaration(ASTNode *node);
void analyze_class_declaration(ASTNode *node);
void analyze_struct_declaration(ASTNode *node);
void analyze_enum_declaration(ASTNode *node);
void analyze_trait_declaration(ASTNode *node);
void analyze_parameter_list(ASTNode *node);

/* Statement Analysis */
void analyze_statement(ASTNode *node);
void analyze_if_statement(ASTNode *node);
void analyze_while_statement(ASTNode *node);
void analyze_for_statement(ASTNode *node);
void analyze_match_statement(ASTNode *node);
void analyze_match_arms(ASTNode *node);
void analyze_return_statement(ASTNode *node);
void analyze_break_statement(ASTNode *node);
void analyze_continue_statement(ASTNode *node);
void analyze_block(ASTNode *node);

/* Expression Analysis */
void analyze_expression(ASTNode *node);
void analyze_identifier(ASTNode *node);
void analyze_binary_expression(ASTNode *node);
void analyze_unary_expression(ASTNode *node);
void analyze_call_expression(ASTNode *node);
void analyze_member_expression(ASTNode *node);
void analyze_index_expression(ASTNode *node);
void analyze_lambda_expression(ASTNode *node);
void analyze_array_literal(ASTNode *node);
void analyze_struct_literal(ASTNode *node);

/* Pattern Analysis */
void analyze_pattern(ASTNode *node);
void analyze_struct_pattern(ASTNode *node);
void analyze_enum_pattern(ASTNode *node);

/* Helper Functions */
int is_lvalue(ASTNode *node);

/* Main Entry Point */
void analyze_program(ASTNode *root);

/* Debugging */
void print_symbol_table(void);
void print_semantic_statistics(void);

#endif /* LSHARP_SEMANTIC_H */

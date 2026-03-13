/* ============================================================================
 * L# Programming Language - Abstract Syntax Tree Header
 * ============================================================================
 * File: ast.h
 * Purpose: AST node type definitions and function declarations
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#ifndef LSHARP_AST_H
#define LSHARP_AST_H

#include <stddef.h>

/* ============================================================================
 * AST NODE TYPES
 * ============================================================================
 */

typedef enum {
    /* Literals */
    AST_INT_LITERAL,
    AST_FLOAT_LITERAL,
    AST_STRING_LITERAL,
    AST_BOOL_LITERAL,
    AST_NULL_LITERAL,
    AST_CHAR_LITERAL,
    AST_ARRAY_LITERAL,
    AST_STRUCT_LITERAL,
    AST_TUPLE_LITERAL,
    
    /* Identifiers and Types */
    AST_IDENTIFIER,
    AST_TYPE,
    AST_PRIMITIVE_TYPE,
    AST_ARRAY_TYPE,
    AST_TUPLE_TYPE,
    AST_FUNCTION_TYPE,
    AST_GENERIC_TYPE,
    AST_REFERENCE_TYPE,
    AST_POINTER_TYPE,
    AST_TRAIT_OBJECT_TYPE,
    AST_IMPL_TRAIT_TYPE,
    AST_NEVER_TYPE,
    
    /* Expressions */
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_TERNARY_EXPR,
    AST_CALL_EXPR,
    AST_MEMBER_EXPR,
    AST_INDEX_EXPR,
    AST_CAST_EXPR,
    AST_LAMBDA_EXPR,
    AST_CLOSURE_EXPR,
    AST_RANGE_EXPR,
    AST_AWAIT_EXPR,
    AST_TRY_OPERATOR_EXPR,
    AST_TYPE_ASCRIPTION,
    AST_DEREF_EXPR,
    AST_ADDRESS_OF_EXPR,
    AST_BOX_EXPR,
    AST_COMPOUND_ASSIGN,
    AST_METHOD_CALL_EXPR,
    AST_FIELD_EXPR,
    
    /* Statements */
    AST_VAR_DECL,
    AST_CONST_DECL,
    AST_FUNC_DECL,
    AST_CLASS_DECL,
    AST_STRUCT_DECL,
    AST_ENUM_DECL,
    AST_TRAIT_DECL,
    AST_IMPL_DECL,
    AST_TYPE_ALIAS_DECL,
    AST_MODULE_DECL,
    AST_USE_DECL,
    AST_EXTERN_DECL,
    
    /* Control Flow */
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_LOOP_STMT,
    AST_MATCH_STMT,
    AST_RETURN_STMT,
    AST_BREAK_STMT,
    AST_CONTINUE_STMT,
    AST_EXPR_STMT,
    AST_BLOCK,
    AST_TRY_STMT,
    AST_THROW_STMT,
    AST_ASYNC_BLOCK,
    AST_UNSAFE_BLOCK,
    AST_CONST_BLOCK,
    AST_LABELED_BLOCK,
    AST_LABELED_LOOP,
    AST_IF_LET_EXPR,
    AST_WHILE_LET_LOOP,
    
    /* Patterns */
    AST_MATCH_ARM,
    AST_RANGE_PATTERN,
    AST_STRUCT_PATTERN,
    AST_ENUM_PATTERN,
    AST_TUPLE_PATTERN,
    AST_SLICE_PATTERN,
    AST_OR_PATTERN,
    AST_BINDING_PATTERN,
    AST_WILDCARD_PATTERN,
    AST_REST_PATTERN,
    
    /* Lists */
    AST_STATEMENT_LIST,
    AST_EXPR_LIST,
    AST_PARAM_LIST,
    AST_PARAM,
    AST_MATCH_ARM_LIST,
    AST_TYPE_LIST,
    AST_GENERIC_PARAM_LIST,
    AST_TRAIT_BOUND_LIST,
    AST_WHERE_CLAUSE,
    
    /* Macros and Attributes */
    AST_MACRO_INVOCATION,
    AST_MACRO_DEFINITION,
    AST_ATTRIBUTE,
    AST_DERIVE_ATTRIBUTE,
    
    /* Miscellaneous */
    AST_PROGRAM,
    AST_ERROR
} ASTNodeType;

/* ============================================================================
 * FORWARD DECLARATIONS
 * ============================================================================
 */

typedef struct ASTNode ASTNode;
typedef struct Type Type;
typedef struct Symbol Symbol;

/* ============================================================================
 * AST NODE DATA STRUCTURES
 * ============================================================================
 */

/* Integer Literal */
typedef struct {
    long long value;
    char *suffix;
} IntLiteralData;

/* Float Literal */
typedef struct {
    double value;
    char *suffix;
} FloatLiteralData;

/* String Literal */
typedef struct {
    char *value;
    size_t length;
    int is_raw;
} StringLiteralData;

/* Boolean Literal */
typedef struct {
    int value;
} BoolLiteralData;

/* Character Literal */
typedef struct {
    char value;
} CharLiteralData;

/* Identifier */
typedef struct {
    char *name;
    Symbol *resolved_symbol;
} IdentifierData;

/* Type Node */
typedef struct {
    char *name;
    ASTNode **params;
    int param_count;
} TypeNodeData;

/* Primitive Type */
typedef struct {
    char *name;
} PrimitiveTypeData;

/* Array Type */
typedef struct {
    ASTNode *element_type;
    ASTNode *size;
} ArrayTypeData;

/* Generic Type */
typedef struct {
    char *name;
    ASTNode **type_args;
    int arg_count;
} GenericTypeData;

/* Binary Expression */
typedef struct {
    char *operator;
    ASTNode *left;
    ASTNode *right;
    Type *result_type;
} BinaryExprData;

/* Unary Expression */
typedef struct {
    char *operator;
    ASTNode *operand;
    int is_prefix;
} UnaryExprData;

/* Ternary Expression */
typedef struct {
    ASTNode *condition;
    ASTNode *then_expr;
    ASTNode *else_expr;
} TernaryExprData;

/* Call Expression */
typedef struct {
    ASTNode *callee;
    ASTNode *arguments;
    int arg_count;
} CallExprData;

/* Member Expression */
typedef struct {
    ASTNode *object;
    char *member;
    int is_computed;
} MemberExprData;

/* Index Expression */
typedef struct {
    ASTNode *array;
    ASTNode *index;
} IndexExprData;

/* Cast Expression */
typedef struct {
    ASTNode *expression;
    ASTNode *target_type;
    int is_safe;
} CastExprData;

/* Lambda Expression */
typedef struct {
    ASTNode *parameters;
    ASTNode *body;
    int has_block_body;
    ASTNode *captures;
} LambdaExprData;

/* Variable Declaration */
typedef struct {
    char *name;
    ASTNode *type;
    ASTNode *initializer;
    int is_mutable;
} VarDeclData;

/* Constant Declaration */
typedef struct {
    char *name;
    ASTNode *type;
    ASTNode *initializer;
} ConstDeclData;

/* Function Declaration */
typedef struct {
    char *name;
    ASTNode *parameters;
    ASTNode *return_type;
    ASTNode *body;
    int is_async;
    int is_generic;
    ASTNode *generic_params;
    ASTNode *where_clause;
} FuncDeclData;

/* Class Declaration */
typedef struct {
    char *name;
    ASTNode *superclass;
    ASTNode *interfaces;
    ASTNode *body;
    ASTNode *generic_params;
} ClassDeclData;

/* Struct Declaration */
typedef struct {
    char *name;
    ASTNode *fields;
    int is_packed;
    ASTNode *generic_params;
} StructDeclData;

/* Enum Declaration */
typedef struct {
    char *name;
    ASTNode *variants;
    ASTNode *underlying_type;
} EnumDeclData;

/* Trait Declaration */
typedef struct {
    char *name;
    ASTNode *methods;
    ASTNode *supertraits;
    ASTNode *generic_params;
} TraitDeclData;

/* If Statement */
typedef struct {
    ASTNode *condition;
    ASTNode *then_branch;
    ASTNode *else_branch;
} IfStmtData;

/* While Statement */
typedef struct {
    ASTNode *condition;
    ASTNode *body;
    char *label;
} WhileStmtData;

/* For Statement */
typedef struct {
    char *iterator;
    ASTNode *iterable;
    ASTNode *body;
    char *label;
} ForStmtData;

/* Match Statement */
typedef struct {
    ASTNode *expression;
    ASTNode *arms;
    int is_exhaustive;
} MatchStmtData;

/* Return Statement */
typedef struct {
    ASTNode *value;
} ReturnStmtData;

/* Break Statement */
typedef struct {
    char *label;
    ASTNode *value;
} BreakStmtData;

/* Continue Statement */
typedef struct {
    char *label;
} ContinueStmtData;

/* Expression Statement */
typedef struct {
    ASTNode *expression;
} ExprStmtData;

/* Block */
typedef struct {
    ASTNode *statements;
    int statement_count;
    int scope_id;
} BlockData;

/* Match Arm */
typedef struct {
    ASTNode *pattern;
    ASTNode *guard;
    ASTNode *body;
} MatchArmData;

/* Range Pattern */
typedef struct {
    ASTNode *start;
    ASTNode *end;
    int is_inclusive;
} RangePatternData;

/* Struct Pattern */
typedef struct {
    char *name;
    ASTNode *fields;
    int has_rest;
} StructPatternData;

/* Enum Pattern */
typedef struct {
    char *enum_name;
    char *variant;
    ASTNode *fields;
} EnumPatternData;

/* Or Pattern */
typedef struct {
    ASTNode *left;
    ASTNode *right;
} OrPatternData;

/* Binding Pattern */
typedef struct {
    char *name;
    ASTNode *pattern;
} BindingPatternData;

/* Statement List */
typedef struct {
    ASTNode **statements;
    int count;
    int capacity;
} StatementListData;

/* Expression List */
typedef struct {
    ASTNode **expressions;
    int count;
    int capacity;
} ExprListData;

/* Parameter List */
typedef struct {
    ASTNode **parameters;
    int count;
    int capacity;
} ParamListData;

/* Parameter */
typedef struct {
    char *name;
    ASTNode *type;
    ASTNode *default_value;
    int is_variadic;
} ParamData;

/* Match Arm List */
typedef struct {
    ASTNode **arms;
    int count;
    int capacity;
} MatchArmListData;

/* Array Literal */
typedef struct {
    ASTNode *elements;
    int element_count;
} ArrayLiteralData;

/* Struct Literal */
typedef struct {
    char *name;
    ASTNode *fields;
    int field_count;
} StructLiteralData;

/* ============================================================================
 * MAIN AST NODE STRUCTURE
 * ============================================================================
 */

struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    
    union {
        IntLiteralData int_literal;
        FloatLiteralData float_literal;
        StringLiteralData string_literal;
        BoolLiteralData bool_literal;
        CharLiteralData char_literal;
        IdentifierData identifier;
        TypeNodeData type_node;
        PrimitiveTypeData primitive_type;
        ArrayTypeData array_type;
        GenericTypeData generic_type;
        BinaryExprData binary_expr;
        UnaryExprData unary_expr;
        TernaryExprData ternary_expr;
        CallExprData call_expr;
        MemberExprData member_expr;
        IndexExprData index_expr;
        CastExprData cast_expr;
        LambdaExprData lambda_expr;
        VarDeclData var_decl;
        ConstDeclData const_decl;
        FuncDeclData func_decl;
        ClassDeclData class_decl;
        StructDeclData struct_decl;
        EnumDeclData enum_decl;
        TraitDeclData trait_decl;
        IfStmtData if_stmt;
        WhileStmtData while_stmt;
        ForStmtData for_stmt;
        MatchStmtData match_stmt;
        ReturnStmtData return_stmt;
        BreakStmtData break_stmt;
        ContinueStmtData continue_stmt;
        ExprStmtData expr_stmt;
        BlockData block;
        MatchArmData match_arm;
        RangePatternData range_pattern;
        StructPatternData struct_pattern;
        EnumPatternData enum_pattern;
        OrPatternData or_pattern;
        BindingPatternData binding_pattern;
        StatementListData statement_list;
        ExprListData expr_list;
        ParamListData param_list;
        ParamData param;
        MatchArmListData match_arm_list;
        ArrayLiteralData array_literal;
        StructLiteralData struct_literal;
    } data;
};

/* ============================================================================
 * FUNCTION DECLARATIONS
 * ============================================================================
 */

/* Memory Management */
ASTNode* allocate_node(ASTNodeType type);
void free_ast_node(ASTNode *node);

/* Literal Node Creation */
ASTNode* create_int_literal(long long value);
ASTNode* create_float_literal(double value);
ASTNode* create_string_literal(const char *value);
ASTNode* create_bool_literal(int value);
ASTNode* create_null_literal(void);
ASTNode* create_char_literal(char value);

/* Identifier and Type Nodes */
ASTNode* create_identifier(const char *name);
ASTNode* create_type_node(const char *name);
ASTNode* create_primitive_type(const char *name);
ASTNode* create_array_type(ASTNode *element_type);
ASTNode* create_generic_type(const char *name, ASTNode *type_arg);

/* Expression Nodes */
ASTNode* create_binary_expr(const char *op, ASTNode *left, ASTNode *right);
ASTNode* create_unary_expr(const char *op, ASTNode *operand);
ASTNode* create_call_expr(ASTNode *callee, ASTNode *args);
ASTNode* create_member_expr(ASTNode *object, const char *member);
ASTNode* create_index_expr(ASTNode *array, ASTNode *index);
ASTNode* create_ternary_expr(ASTNode *condition, ASTNode *then_expr, ASTNode *else_expr);
ASTNode* create_cast_expr(ASTNode *expr, ASTNode *target_type);
ASTNode* create_lambda_expr(ASTNode *params, ASTNode *body, int has_block);

/* Statement Nodes */
ASTNode* create_var_decl(const char *name, ASTNode *type, ASTNode *init, int is_mutable);
ASTNode* create_const_decl(const char *name, ASTNode *init);
ASTNode* create_func_decl(const char *name, ASTNode *params, ASTNode *return_type, 
                          ASTNode *body, int is_async);
ASTNode* create_class_decl(const char *name, ASTNode *superclass, ASTNode *body);
ASTNode* create_struct_decl(const char *name, ASTNode *fields);
ASTNode* create_enum_decl(const char *name, ASTNode *variants);
ASTNode* create_trait_decl(const char *name, ASTNode *methods);

/* Control Flow Nodes */
ASTNode* create_if_stmt(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode* create_while_stmt(ASTNode *condition, ASTNode *body);
ASTNode* create_for_stmt(const char *iterator, ASTNode *iterable, ASTNode *body);
ASTNode* create_match_stmt(ASTNode *expr, ASTNode *arms);
ASTNode* create_return_stmt(ASTNode *value);
ASTNode* create_break_stmt(void);
ASTNode* create_continue_stmt(void);
ASTNode* create_expr_stmt(ASTNode *expr);
ASTNode* create_block(ASTNode *statements);

/* Pattern Matching Nodes */
ASTNode* create_match_arm(ASTNode *pattern, ASTNode *body);
ASTNode* create_range_pattern(ASTNode *start, ASTNode *end, int inclusive);
ASTNode* create_struct_pattern(const char *name, ASTNode *fields);
ASTNode* create_enum_pattern(const char *enum_name, const char *variant, ASTNode *fields);
ASTNode* create_or_pattern(ASTNode *left, ASTNode *right);
ASTNode* create_binding_pattern(const char *name, ASTNode *pattern);

/* List Construction Helpers */
ASTNode* create_statement_list(ASTNode *stmt);
ASTNode* append_statement(ASTNode *list, ASTNode *stmt);
ASTNode* create_param_list(ASTNode *param);
ASTNode* append_param(ASTNode *list, ASTNode *param);
ASTNode* create_param(const char *name, ASTNode *type, ASTNode *default_value);
ASTNode* create_expr_list(ASTNode *expr);
ASTNode* append_expr(ASTNode *list, ASTNode *expr);
ASTNode* create_arg_list(ASTNode *arg);
ASTNode* append_arg(ASTNode *list, ASTNode *arg);
ASTNode* create_match_arm_list(ASTNode *arm);
ASTNode* append_match_arm(ASTNode *list, ASTNode *arm);

/* Array and Struct Literals */
ASTNode* create_array_literal(ASTNode *elements);
ASTNode* create_struct_literal(const char *name, ASTNode *fields);

/* AST Traversal */
typedef void (*ASTVisitorFunc)(ASTNode *node, void *context);
void traverse_ast_preorder(ASTNode *node, ASTVisitorFunc visitor, void *context);
void traverse_ast_postorder(ASTNode *node, ASTVisitorFunc visitor, void *context);

/* AST Cloning */
ASTNode* clone_ast_node(ASTNode *node);

/* AST Printing and Debugging */
void print_ast_node(ASTNode *node, int indent);
void print_ast_json(ASTNode *node, FILE *output, int indent);

/* AST Statistics */
void print_ast_statistics(void);
size_t count_ast_nodes(ASTNode *node);
int get_ast_depth(ASTNode *node);

#endif /* LSHARP_AST_H */

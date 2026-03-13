/* L# Type Checker Header */

#ifndef LSHARP_TYPECHECKER_H
#define LSHARP_TYPECHECKER_H

#include "ast.h"

/* Type Representation */
typedef struct Type Type;

/* Type Environment */
typedef struct TypeEnv TypeEnv;

TypeEnv* create_type_env(TypeEnv *parent);
void enter_type_scope(void);
void exit_type_scope(void);
void bind_type(const char *name, Type *type);
Type* lookup_type(const char *name);

/* Type Construction */
Type* create_primitive_type_obj(const char *name);
Type* create_function_type_obj(Type **param_types, int param_count, Type *return_type);
Type* create_array_type_obj(Type *element_type);
Type* create_reference_type_obj(Type *inner_type, int is_mutable, const char *lifetime);
Type* create_generic_type_obj(const char *name, Type **type_args, int arg_count);

/* Type Inference */
Type* infer_expression_type(ASTNode *node);
Type* infer_literal_type(ASTNode *node);
Type* infer_binary_expr_type(ASTNode *node);

/* Type Unification */
int types_equal(Type *t1, Type *t2);
Type* unify_types(Type *t1, Type *t2);
Type* unify_numeric_types(Type *t1, Type *t2);

/* Trait Resolution */
void register_trait_impl(const char *trait_name, const char *type_name, ASTNode *methods);
int type_implements_trait(Type *type, const char *trait_name);
void check_trait_bounds(Type *type, ASTNode *bounds);

/* Lifetime Checking */
void register_lifetime(const char *name, int start, int end);
int lifetime_outlives(const char *longer, const char *shorter);
void check_lifetime_bounds(Type *type);

/* Borrow Checking */
typedef enum {
    BORROW_NONE,
    BORROW_SHARED,
    BORROW_MUTABLE
} BorrowKind;

void register_borrow(const char *var, BorrowKind kind, int line);
void release_borrow(const char *var);

/* Type Checking Main Functions */
void typecheck_expression(ASTNode *node);
void typecheck_statement(ASTNode *node);
void typecheck_program(ASTNode *root);

/* Type Pretty Printing */
void print_type(Type *type);
void print_type_error(const char *expected, const char *got, int line);

#endif /* LSHARP_TYPECHECKER_H */

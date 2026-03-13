/* L# Type Checker Implementation */
/* Advanced type inference and checking */

#include "typechecker.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * TYPE REPRESENTATION
 * ============================================================================
 */

typedef enum {
    TYPE_PRIMITIVE,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_TRAIT,
    TYPE_FUNCTION,
    TYPE_ARRAY,
    TYPE_TUPLE,
    TYPE_REFERENCE,
    TYPE_POINTER,
    TYPE_GENERIC,
    TYPE_ASSOCIATED,
    TYPE_EXISTENTIAL,
    TYPE_UNION,
    TYPE_INTERSECTION
} TypeKind;

typedef struct Type {
    TypeKind kind;
    char *name;
    struct Type **params;
    int param_count;
    int is_mutable;
    char *lifetime;
} Type;

/* ============================================================================
 * TYPE ENVIRONMENT
 * ============================================================================
 */

typedef struct TypeBinding {
    char *name;
    Type *type;
    struct TypeBinding *next;
} TypeBinding;

typedef struct TypeEnv {
    TypeBinding *bindings;
    struct TypeEnv *parent;
} TypeEnv;

static TypeEnv *current_env = NULL;

TypeEnv* create_type_env(TypeEnv *parent) {
    TypeEnv *env = malloc(sizeof(TypeEnv));
    env->bindings = NULL;
    env->parent = parent;
    return env;
}

void enter_type_scope() {
    current_env = create_type_env(current_env);
}

void exit_type_scope() {
    if (current_env) {
        TypeEnv *old = current_env;
        current_env = current_env->parent;
        free(old);
    }
}

void bind_type(const char *name, Type *type) {
    TypeBinding *binding = malloc(sizeof(TypeBinding));
    binding->name = strdup(name);
    binding->type = type;
    binding->next = current_env->bindings;
    current_env->bindings = binding;
}

Type* lookup_type(const char *name) {
    TypeEnv *env = current_env;
    while (env) {
        TypeBinding *binding = env->bindings;
        while (binding) {
            if (strcmp(binding->name, name) == 0) {
                return binding->type;
            }
            binding = binding->next;
        }
        env = env->parent;
    }
    return NULL;
}

/* ============================================================================
 * TYPE CONSTRUCTION
 * ============================================================================
 */

Type* create_primitive_type_obj(const char *name) {
    Type *type = malloc(sizeof(Type));
    type->kind = TYPE_PRIMITIVE;
    type->name = strdup(name);
    type->params = NULL;
    type->param_count = 0;
    type->is_mutable = 0;
    type->lifetime = NULL;
    return type;
}

Type* create_function_type_obj(Type **param_types, int param_count, Type *return_type) {
    Type *type = malloc(sizeof(Type));
    type->kind = TYPE_FUNCTION;
    type->name = strdup("fn");
    type->params = malloc(sizeof(Type*) * (param_count + 1));
    for (int i = 0; i < param_count; i++) {
        type->params[i] = param_types[i];
    }
    type->params[param_count] = return_type;
    type->param_count = param_count + 1;
    type->is_mutable = 0;
    type->lifetime = NULL;
    return type;
}

Type* create_array_type_obj(Type *element_type) {
    Type *type = malloc(sizeof(Type));
    type->kind = TYPE_ARRAY;
    type->name = strdup("array");
    type->params = malloc(sizeof(Type*));
    type->params[0] = element_type;
    type->param_count = 1;
    type->is_mutable = 0;
    type->lifetime = NULL;
    return type;
}

Type* create_reference_type_obj(Type *inner_type, int is_mutable, const char *lifetime) {
    Type *type = malloc(sizeof(Type));
    type->kind = TYPE_REFERENCE;
    type->name = strdup("ref");
    type->params = malloc(sizeof(Type*));
    type->params[0] = inner_type;
    type->param_count = 1;
    type->is_mutable = is_mutable;
    type->lifetime = lifetime ? strdup(lifetime) : NULL;
    return type;
}

Type* create_generic_type_obj(const char *name, Type **type_args, int arg_count) {
    Type *type = malloc(sizeof(Type));
    type->kind = TYPE_GENERIC;
    type->name = strdup(name);
    type->params = malloc(sizeof(Type*) * arg_count);
    for (int i = 0; i < arg_count; i++) {
        type->params[i] = type_args[i];
    }
    type->param_count = arg_count;
    type->is_mutable = 0;
    type->lifetime = NULL;
    return type;
}

/* ============================================================================
 * TYPE INFERENCE
 * ============================================================================
 */

Type* infer_literal_type(ASTNode *node) {
    switch (node->type) {
        case AST_INT_LITERAL:
            return create_primitive_type_obj("i32");
        case AST_FLOAT_LITERAL:
            return create_primitive_type_obj("f64");
        case AST_STRING_LITERAL:
            return create_primitive_type_obj("string");
        case AST_BOOL_LITERAL:
            return create_primitive_type_obj("bool");
        case AST_NULL_LITERAL:
            return create_primitive_type_obj("null");
        default:
            return NULL;
    }
}

Type* infer_binary_expr_type(ASTNode *node) {
    Type *left_type = infer_expression_type(node->data.binary_expr.left);
    Type *right_type = infer_expression_type(node->data.binary_expr.right);
    
    const char *op = node->data.binary_expr.operator;
    
    /* Arithmetic operators */
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
        strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
        if (types_equal(left_type, right_type)) {
            return left_type;
        }
        /* Try numeric coercion */
        return unify_numeric_types(left_type, right_type);
    }
    
    /* Comparison operators */
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
        strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
        strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
        return create_primitive_type_obj("bool");
    }
    
    /* Logical operators */
    if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        return create_primitive_type_obj("bool");
    }
    
    return NULL;
}

Type* infer_expression_type(ASTNode *node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case AST_INT_LITERAL:
        case AST_FLOAT_LITERAL:
        case AST_STRING_LITERAL:
        case AST_BOOL_LITERAL:
        case AST_NULL_LITERAL:
            return infer_literal_type(node);
            
        case AST_IDENTIFIER: {
            const char *name = node->data.identifier.name;
            return lookup_type(name);
        }
        
        case AST_BINARY_EXPR:
            return infer_binary_expr_type(node);
            
        case AST_UNARY_EXPR: {
            Type *operand_type = infer_expression_type(node->data.unary_expr.operand);
            const char *op = node->data.unary_expr.operator;
            
            if (strcmp(op, "-") == 0) {
                return operand_type;
            } else if (strcmp(op, "!") == 0) {
                return create_primitive_type_obj("bool");
            } else if (strcmp(op, "&") == 0) {
                return create_reference_type_obj(operand_type, 0, NULL);
            } else if (strcmp(op, "*") == 0) {
                if (operand_type->kind == TYPE_REFERENCE || operand_type->kind == TYPE_POINTER) {
                    return operand_type->params[0];
                }
            }
            return NULL;
        }
        
        case AST_CALL_EXPR: {
            Type *callee_type = infer_expression_type(node->data.call_expr.callee);
            if (callee_type && callee_type->kind == TYPE_FUNCTION) {
                return callee_type->params[callee_type->param_count - 1];
            }
            return NULL;
        }
        
        default:
            return NULL;
    }
}

/* ============================================================================
 * TYPE UNIFICATION
 * ============================================================================
 */

int types_equal(Type *t1, Type *t2) {
    if (!t1 || !t2) return 0;
    if (t1->kind != t2->kind) return 0;
    
    switch (t1->kind) {
        case TYPE_PRIMITIVE:
            return strcmp(t1->name, t2->name) == 0;
            
        case TYPE_ARRAY:
            return types_equal(t1->params[0], t2->params[0]);
            
        case TYPE_FUNCTION:
            if (t1->param_count != t2->param_count) return 0;
            for (int i = 0; i < t1->param_count; i++) {
                if (!types_equal(t1->params[i], t2->params[i])) return 0;
            }
            return 1;
            
        case TYPE_REFERENCE:
            if (t1->is_mutable != t2->is_mutable) return 0;
            return types_equal(t1->params[0], t2->params[0]);
            
        default:
            return 0;
    }
}

Type* unify_types(Type *t1, Type *t2) {
    if (types_equal(t1, t2)) return t1;
    
    /* Try coercion rules */
    if (t1->kind == TYPE_PRIMITIVE && t2->kind == TYPE_PRIMITIVE) {
        return unify_numeric_types(t1, t2);
    }
    
    return NULL;
}

Type* unify_numeric_types(Type *t1, Type *t2) {
    /* Integer promotion rules */
    if (strcmp(t1->name, "i64") == 0 || strcmp(t2->name, "i64") == 0) {
        return create_primitive_type_obj("i64");
    }
    if (strcmp(t1->name, "i32") == 0 || strcmp(t2->name, "i32") == 0) {
        return create_primitive_type_obj("i32");
    }
    
    /* Float promotion rules */
    if (strcmp(t1->name, "f64") == 0 || strcmp(t2->name, "f64") == 0) {
        return create_primitive_type_obj("f64");
    }
    if (strcmp(t1->name, "f32") == 0 || strcmp(t2->name, "f32") == 0) {
        return create_primitive_type_obj("f32");
    }
    
    return t1;
}

/* ============================================================================
 * TRAIT RESOLUTION
 * ============================================================================
 */

typedef struct TraitImpl {
    char *trait_name;
    char *type_name;
    ASTNode *methods;
    struct TraitImpl *next;
} TraitImpl;

static TraitImpl *trait_impls = NULL;

void register_trait_impl(const char *trait_name, const char *type_name, ASTNode *methods) {
    TraitImpl *impl = malloc(sizeof(TraitImpl));
    impl->trait_name = strdup(trait_name);
    impl->type_name = strdup(type_name);
    impl->methods = methods;
    impl->next = trait_impls;
    trait_impls = impl;
}

int type_implements_trait(Type *type, const char *trait_name) {
    TraitImpl *impl = trait_impls;
    while (impl) {
        if (strcmp(impl->trait_name, trait_name) == 0 &&
            strcmp(impl->type_name, type->name) == 0) {
            return 1;
        }
        impl = impl->next;
    }
    return 0;
}

void check_trait_bounds(Type *type, ASTNode *bounds) {
    /* Verify type satisfies all trait bounds */
}

/* ============================================================================
 * LIFETIME CHECKING
 * ============================================================================
 */

typedef struct Lifetime {
    char *name;
    int start_line;
    int end_line;
} Lifetime;

typedef struct LifetimeEnv {
    Lifetime *lifetimes;
    int count;
} LifetimeEnv;

static LifetimeEnv lifetime_env = {NULL, 0};

void register_lifetime(const char *name, int start, int end) {
    Lifetime *lt = malloc(sizeof(Lifetime));
    lt->name = strdup(name);
    lt->start_line = start;
    lt->end_line = end;
    
    lifetime_env.lifetimes = realloc(lifetime_env.lifetimes, 
                                     sizeof(Lifetime) * (lifetime_env.count + 1));
    lifetime_env.lifetimes[lifetime_env.count++] = *lt;
}

int lifetime_outlives(const char *longer, const char *shorter) {
    Lifetime *lt1 = NULL, *lt2 = NULL;
    
    for (int i = 0; i < lifetime_env.count; i++) {
        if (strcmp(lifetime_env.lifetimes[i].name, longer) == 0) {
            lt1 = &lifetime_env.lifetimes[i];
        }
        if (strcmp(lifetime_env.lifetimes[i].name, shorter) == 0) {
            lt2 = &lifetime_env.lifetimes[i];
        }
    }
    
    if (!lt1 || !lt2) return 0;
    
    return lt1->start_line <= lt2->start_line && lt1->end_line >= lt2->end_line;
}

void check_lifetime_bounds(Type *type) {
    if (type->kind == TYPE_REFERENCE && type->lifetime) {
        /* Verify lifetime is valid */
    }
}

/* ============================================================================
 * BORROW CHECKING
 * ============================================================================
 */

typedef enum {
    BORROW_NONE,
    BORROW_SHARED,
    BORROW_MUTABLE
} BorrowKind;

typedef struct Borrow {
    char *variable;
    BorrowKind kind;
    int line;
    struct Borrow *next;
} Borrow;

static Borrow *active_borrows = NULL;

void register_borrow(const char *var, BorrowKind kind, int line) {
    /* Check for conflicting borrows */
    Borrow *b = active_borrows;
    while (b) {
        if (strcmp(b->variable, var) == 0) {
            if (kind == BORROW_MUTABLE || b->kind == BORROW_MUTABLE) {
                fprintf(stderr, "Error: Cannot borrow '%s' as %s while already borrowed\n",
                       var, kind == BORROW_MUTABLE ? "mutable" : "immutable");
                return;
            }
        }
        b = b->next;
    }
    
    Borrow *new_borrow = malloc(sizeof(Borrow));
    new_borrow->variable = strdup(var);
    new_borrow->kind = kind;
    new_borrow->line = line;
    new_borrow->next = active_borrows;
    active_borrows = new_borrow;
}

void release_borrow(const char *var) {
    Borrow **b = &active_borrows;
    while (*b) {
        if (strcmp((*b)->variable, var) == 0) {
            Borrow *to_free = *b;
            *b = (*b)->next;
            free(to_free->variable);
            free(to_free);
            return;
        }
        b = &(*b)->next;
    }
}

/* ============================================================================
 * TYPE CHECKING MAIN FUNCTIONS
 * ============================================================================
 */

void typecheck_expression(ASTNode *node) {
    if (!node) return;
    
    Type *inferred = infer_expression_type(node);
    if (!inferred) {
        fprintf(stderr, "Error: Cannot infer type for expression\n");
    }
}

void typecheck_statement(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_VAR_DECL: {
            Type *init_type = infer_expression_type(node->data.var_decl.initializer);
            if (node->data.var_decl.type) {
                /* Check declared type matches inferred type */
            }
            bind_type(node->data.var_decl.name, init_type);
            break;
        }
        
        case AST_FUNC_DECL:
            enter_type_scope();
            /* Typecheck function body */
            exit_type_scope();
            break;
            
        case AST_IF_STMT:
            typecheck_expression(node->data.if_stmt.condition);
            typecheck_statement(node->data.if_stmt.then_branch);
            if (node->data.if_stmt.else_branch) {
                typecheck_statement(node->data.if_stmt.else_branch);
            }
            break;
            
        default:
            break;
    }
}

void typecheck_program(ASTNode *root) {
    enter_type_scope();
    typecheck_statement(root);
    exit_type_scope();
}

/* ============================================================================
 * TYPE PRETTY PRINTING
 * ============================================================================
 */

void print_type(Type *type) {
    if (!type) {
        printf("unknown");
        return;
    }
    
    switch (type->kind) {
        case TYPE_PRIMITIVE:
            printf("%s", type->name);
            break;
            
        case TYPE_ARRAY:
            printf("[");
            print_type(type->params[0]);
            printf("]");
            break;
            
        case TYPE_FUNCTION:
            printf("fn(");
            for (int i = 0; i < type->param_count - 1; i++) {
                if (i > 0) printf(", ");
                print_type(type->params[i]);
            }
            printf(") -> ");
            print_type(type->params[type->param_count - 1]);
            break;
            
        case TYPE_REFERENCE:
            printf("&");
            if (type->is_mutable) printf("mut ");
            if (type->lifetime) printf("'%s ", type->lifetime);
            print_type(type->params[0]);
            break;
            
        default:
            printf("complex_type");
    }
}

void print_type_error(const char *expected, const char *got, int line) {
    fprintf(stderr, "Type Error at line %d:\n", line);
    fprintf(stderr, "  Expected: %s\n", expected);
    fprintf(stderr, "  Got: %s\n", got);
}

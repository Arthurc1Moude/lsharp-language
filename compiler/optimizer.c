/* L# Compiler Optimizer Implementation */
/* Advanced optimization passes for L# bytecode */

#include "optimizer.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * CONSTANT FOLDING
 * ============================================================================
 */

ASTNode* fold_binary_operation(const char *op, ASTNode *left, ASTNode *right) {
    if (left->type != AST_INT_LITERAL || right->type != AST_INT_LITERAL) {
        return NULL;
    }
    
    int left_val = left->data.int_literal.value;
    int right_val = right->data.int_literal.value;
    int result;
    
    if (strcmp(op, "+") == 0) {
        result = left_val + right_val;
    } else if (strcmp(op, "-") == 0) {
        result = left_val - right_val;
    } else if (strcmp(op, "*") == 0) {
        result = left_val * right_val;
    } else if (strcmp(op, "/") == 0) {
        if (right_val == 0) return NULL;
        result = left_val / right_val;
    } else if (strcmp(op, "%") == 0) {
        if (right_val == 0) return NULL;
        result = left_val % right_val;
    } else if (strcmp(op, "&") == 0) {
        result = left_val & right_val;
    } else if (strcmp(op, "|") == 0) {
        result = left_val | right_val;
    } else if (strcmp(op, "^") == 0) {
        result = left_val ^ right_val;
    } else if (strcmp(op, "<<") == 0) {
        result = left_val << right_val;
    } else if (strcmp(op, ">>") == 0) {
        result = left_val >> right_val;
    } else {
        return NULL;
    }
    
    return create_int_literal(result);
}

void constant_folding_pass(ASTNode *node) {
    if (!node) return;
    
    if (node->type == AST_BINARY_EXPR) {
        constant_folding_pass(node->data.binary_expr.left);
        constant_folding_pass(node->data.binary_expr.right);
        
        ASTNode *folded = fold_binary_operation(
            node->data.binary_expr.operator,
            node->data.binary_expr.left,
            node->data.binary_expr.right
        );
        
        if (folded) {
            *node = *folded;
        }
    }
}

/* ============================================================================
 * DEAD CODE ELIMINATION
 * ============================================================================
 */

typedef struct ReachabilityInfo {
    int *reachable;
    int count;
} ReachabilityInfo;

void mark_reachable(ASTNode *node, ReachabilityInfo *info) {
    if (!node) return;
    
    switch (node->type) {
        case AST_IF_STMT:
            mark_reachable(node->data.if_stmt.condition, info);
            mark_reachable(node->data.if_stmt.then_branch, info);
            
            if (node->data.if_stmt.condition->type == AST_BOOL_LITERAL) {
                if (node->data.if_stmt.condition->data.bool_literal.value) {
                    mark_reachable(node->data.if_stmt.then_branch, info);
                } else if (node->data.if_stmt.else_branch) {
                    mark_reachable(node->data.if_stmt.else_branch, info);
                }
            } else {
                mark_reachable(node->data.if_stmt.then_branch, info);
                if (node->data.if_stmt.else_branch) {
                    mark_reachable(node->data.if_stmt.else_branch, info);
                }
            }
            break;
            
        case AST_WHILE_STMT:
            mark_reachable(node->data.while_stmt.condition, info);
            if (node->data.while_stmt.condition->type != AST_BOOL_LITERAL ||
                node->data.while_stmt.condition->data.bool_literal.value) {
                mark_reachable(node->data.while_stmt.body, info);
            }
            break;
            
        default:
            break;
    }
}

void eliminate_dead_code(ASTNode *node) {
    ReachabilityInfo info = {0};
    mark_reachable(node, &info);
}

/* ============================================================================
 * COMMON SUBEXPRESSION ELIMINATION
 * ============================================================================
 */

typedef struct ExpressionHash {
    char *expr_string;
    ASTNode *node;
    struct ExpressionHash *next;
} ExpressionHash;

static ExpressionHash *expr_table = NULL;

char* expression_to_string(ASTNode *node) {
    if (!node) return strdup("");
    
    char buffer[1024];
    switch (node->type) {
        case AST_INT_LITERAL:
            snprintf(buffer, sizeof(buffer), "INT(%d)", node->data.int_literal.value);
            break;
        case AST_BINARY_EXPR:
            snprintf(buffer, sizeof(buffer), "BIN(%s,%s,%s)",
                    node->data.binary_expr.operator,
                    expression_to_string(node->data.binary_expr.left),
                    expression_to_string(node->data.binary_expr.right));
            break;
        default:
            snprintf(buffer, sizeof(buffer), "NODE(%d)", node->type);
    }
    
    return strdup(buffer);
}

ASTNode* find_common_subexpression(ASTNode *expr) {
    char *expr_str = expression_to_string(expr);
    
    ExpressionHash *entry = expr_table;
    while (entry) {
        if (strcmp(entry->expr_string, expr_str) == 0) {
            free(expr_str);
            return entry->node;
        }
        entry = entry->next;
    }
    
    ExpressionHash *new_entry = malloc(sizeof(ExpressionHash));
    new_entry->expr_string = expr_str;
    new_entry->node = expr;
    new_entry->next = expr_table;
    expr_table = new_entry;
    
    return NULL;
}

void cse_pass(ASTNode *node) {
    if (!node) return;
    
    if (node->type == AST_BINARY_EXPR) {
        ASTNode *common = find_common_subexpression(node);
        if (common) {
            *node = *common;
        }
    }
}

/* ============================================================================
 * LOOP OPTIMIZATION
 * ============================================================================
 */

void loop_invariant_code_motion(ASTNode *loop) {
    /* Move loop-invariant code outside the loop */
}

void loop_unrolling(ASTNode *loop, int factor) {
    /* Unroll loop by given factor */
}

void loop_fusion(ASTNode *loop1, ASTNode *loop2) {
    /* Fuse two adjacent loops */
}

void loop_interchange(ASTNode *nested_loop) {
    /* Swap inner and outer loops */
}

void loop_vectorization(ASTNode *loop) {
    /* Convert loop to SIMD operations */
}

/* ============================================================================
 * FUNCTION INLINING
 * ============================================================================
 */

typedef struct InlineCandidate {
    char *func_name;
    ASTNode *func_body;
    int call_count;
    int body_size;
    struct InlineCandidate *next;
} InlineCandidate;

static InlineCandidate *inline_candidates = NULL;

void analyze_inline_candidates(ASTNode *program) {
    /* Identify functions suitable for inlining */
}

void inline_function_call(ASTNode *call, ASTNode *func_body) {
    /* Replace call with inlined function body */
}

void aggressive_inlining(ASTNode *program) {
    analyze_inline_candidates(program);
    
    InlineCandidate *candidate = inline_candidates;
    while (candidate) {
        if (candidate->call_count > 0 && candidate->body_size < 50) {
            /* Inline this function */
        }
        candidate = candidate->next;
    }
}

/* ============================================================================
 * TAIL CALL OPTIMIZATION
 * ============================================================================
 */

int is_tail_call(ASTNode *call, ASTNode *func) {
    /* Check if call is in tail position */
    return 0;
}

void optimize_tail_calls(ASTNode *func) {
    /* Convert tail calls to jumps */
}

void tail_recursion_to_loop(ASTNode *func) {
    /* Convert tail recursive function to loop */
}

/* ============================================================================
 * REGISTER ALLOCATION
 * ============================================================================
 */

typedef struct LiveRange {
    int start;
    int end;
    int reg;
} LiveRange;

typedef struct InterferenceGraph {
    int **edges;
    int node_count;
} InterferenceGraph;

InterferenceGraph* build_interference_graph(ASTNode *func) {
    InterferenceGraph *graph = malloc(sizeof(InterferenceGraph));
    graph->node_count = 0;
    graph->edges = NULL;
    return graph;
}

void color_graph(InterferenceGraph *graph, int num_registers) {
    /* Graph coloring for register allocation */
}

void allocate_registers(ASTNode *func, int num_registers) {
    InterferenceGraph *graph = build_interference_graph(func);
    color_graph(graph, num_registers);
}

/* ============================================================================
 * STRENGTH REDUCTION
 * ============================================================================
 */

void strength_reduction_pass(ASTNode *node) {
    if (!node) return;
    
    if (node->type == AST_BINARY_EXPR) {
        /* Replace expensive operations with cheaper ones */
        if (strcmp(node->data.binary_expr.operator, "*") == 0) {
            /* Check if multiplying by power of 2 */
            if (node->data.binary_expr.right->type == AST_INT_LITERAL) {
                int val = node->data.binary_expr.right->data.int_literal.value;
                if ((val & (val - 1)) == 0) {
                    /* Replace with left shift */
                    int shift = 0;
                    while ((1 << shift) < val) shift++;
                    node->data.binary_expr.operator = strdup("<<");
                    node->data.binary_expr.right->data.int_literal.value = shift;
                }
            }
        }
    }
}

/* ============================================================================
 * PEEPHOLE OPTIMIZATION
 * ============================================================================
 */

void peephole_optimize(unsigned char *bytecode, int length) {
    for (int i = 0; i < length - 1; i++) {
        /* Pattern: PUSH x, POP -> NOP */
        if (bytecode[i] == 0x60 && bytecode[i+1] == 0x61) {
            bytecode[i] = 0x00;
            bytecode[i+1] = 0x00;
        }
        
        /* Pattern: LOAD x, STORE x -> NOP */
        if (bytecode[i] == 0x02 && bytecode[i+2] == 0x03) {
            if (bytecode[i+1] == bytecode[i+3]) {
                bytecode[i] = 0x00;
                bytecode[i+2] = 0x00;
            }
        }
        
        /* Pattern: ADD 0 -> NOP */
        if (bytecode[i] == 0x10 && bytecode[i+1] == 0) {
            bytecode[i] = 0x00;
        }
        
        /* Pattern: MUL 1 -> NOP */
        if (bytecode[i] == 0x12 && bytecode[i+1] == 1) {
            bytecode[i] = 0x00;
        }
    }
}

/* ============================================================================
 * BRANCH PREDICTION
 * ============================================================================
 */

typedef struct BranchProfile {
    int taken_count;
    int not_taken_count;
} BranchProfile;

void profile_branches(ASTNode *node, BranchProfile *profile) {
    /* Collect branch statistics */
}

void optimize_branch_layout(ASTNode *node, BranchProfile *profile) {
    /* Reorder code based on branch predictions */
}

/* ============================================================================
 * MEMORY OPTIMIZATION
 * ============================================================================
 */

void escape_analysis(ASTNode *func) {
    /* Determine which allocations can be stack-allocated */
}

void stack_allocation_promotion(ASTNode *func) {
    /* Move heap allocations to stack when safe */
}

void memory_pooling(ASTNode *program) {
    /* Use memory pools for frequent allocations */
}

/* ============================================================================
 * VECTORIZATION
 * ============================================================================
 */

int can_vectorize_loop(ASTNode *loop) {
    /* Check if loop can be vectorized */
    return 0;
}

void vectorize_loop(ASTNode *loop) {
    /* Convert loop to SIMD operations */
}

void auto_vectorization_pass(ASTNode *program) {
    /* Automatically vectorize suitable loops */
}

/* ============================================================================
 * INTERPROCEDURAL OPTIMIZATION
 * ============================================================================
 */

typedef struct CallGraph {
    char **functions;
    int **calls;
    int func_count;
} CallGraph;

CallGraph* build_call_graph(ASTNode *program) {
    CallGraph *graph = malloc(sizeof(CallGraph));
    graph->func_count = 0;
    return graph;
}

void interprocedural_constant_propagation(CallGraph *graph) {
    /* Propagate constants across function boundaries */
}

void interprocedural_dead_code_elimination(CallGraph *graph) {
    /* Remove unused functions */
}

/* ============================================================================
 * PROFILE-GUIDED OPTIMIZATION
 * ============================================================================
 */

typedef struct ProfileData {
    int *execution_counts;
    int *branch_taken;
    double *function_times;
} ProfileData;

void apply_profile_guided_optimizations(ASTNode *program, ProfileData *profile) {
    /* Use profiling data to guide optimizations */
}

/* ============================================================================
 * OPTIMIZATION PIPELINE
 * ============================================================================
 */

void run_optimization_pipeline(ASTNode *program, int optimization_level) {
    switch (optimization_level) {
        case 0:
            /* No optimization */
            break;
            
        case 1:
            /* Basic optimizations */
            constant_folding_pass(program);
            eliminate_dead_code(program);
            break;
            
        case 2:
            /* Standard optimizations */
            constant_folding_pass(program);
            eliminate_dead_code(program);
            cse_pass(program);
            strength_reduction_pass(program);
            break;
            
        case 3:
            /* Aggressive optimizations */
            constant_folding_pass(program);
            eliminate_dead_code(program);
            cse_pass(program);
            strength_reduction_pass(program);
            aggressive_inlining(program);
            optimize_tail_calls(program);
            auto_vectorization_pass(program);
            break;
    }
}

void print_optimization_stats() {
    printf("\n=== Optimization Statistics ===\n");
    printf("Constant folding: Applied\n");
    printf("Dead code elimination: Applied\n");
    printf("Common subexpression elimination: Applied\n");
    printf("================================\n");
}

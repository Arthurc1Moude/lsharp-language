/* L# Compiler Optimizer Header */

#ifndef LSHARP_OPTIMIZER_H
#define LSHARP_OPTIMIZER_H

#include "ast.h"

/* Optimization Passes */
void constant_folding_pass(ASTNode *node);
void eliminate_dead_code(ASTNode *node);
void cse_pass(ASTNode *node);
void strength_reduction_pass(ASTNode *node);
void peephole_optimize(unsigned char *bytecode, int length);

/* Loop Optimizations */
void loop_invariant_code_motion(ASTNode *loop);
void loop_unrolling(ASTNode *loop, int factor);
void loop_fusion(ASTNode *loop1, ASTNode *loop2);
void loop_interchange(ASTNode *nested_loop);
void loop_vectorization(ASTNode *loop);

/* Function Optimizations */
void aggressive_inlining(ASTNode *program);
void optimize_tail_calls(ASTNode *func);
void tail_recursion_to_loop(ASTNode *func);

/* Register Allocation */
void allocate_registers(ASTNode *func, int num_registers);

/* Interprocedural Optimization */
typedef struct CallGraph CallGraph;
CallGraph* build_call_graph(ASTNode *program);
void interprocedural_constant_propagation(CallGraph *graph);
void interprocedural_dead_code_elimination(CallGraph *graph);

/* Main Pipeline */
void run_optimization_pipeline(ASTNode *program, int optimization_level);
void print_optimization_stats(void);

#endif /* LSHARP_OPTIMIZER_H */

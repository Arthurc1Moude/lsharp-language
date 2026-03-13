/* Dataflow Analysis - Real Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Reaching Definitions Analysis */
typedef struct {
    uint64_t *gen;
    uint64_t *kill;
    uint64_t *in;
    uint64_t *out;
} ReachingDefs;

ReachingDefs* create_reaching_defs(int num_blocks, int num_defs) {
    ReachingDefs *rd = malloc(sizeof(ReachingDefs));
    int size = (num_defs + 63) / 64;
    rd->gen = calloc(num_blocks * size, sizeof(uint64_t));
    rd->kill = calloc(num_blocks * size, sizeof(uint64_t));
    rd->in = calloc(num_blocks * size, sizeof(uint64_t));
    rd->out = calloc(num_blocks * size, sizeof(uint64_t));
    return rd;
}

void set_bit(uint64_t *set, int bit) {
    set[bit / 64] |= (1ULL << (bit % 64));
}

int test_bit(uint64_t *set, int bit) {
    return (set[bit / 64] & (1ULL << (bit % 64))) != 0;
}

void union_sets(uint64_t *dest, uint64_t *src, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] |= src[i];
    }
}

/* Live Variable Analysis */
typedef struct {
    uint64_t *use;
    uint64_t *def;
    uint64_t *live_in;
    uint64_t *live_out;
} LiveVars;

LiveVars* create_live_vars(int num_blocks, int num_vars) {
    LiveVars *lv = malloc(sizeof(LiveVars));
    int size = (num_vars + 63) / 64;
    lv->use = calloc(num_blocks * size, sizeof(uint64_t));
    lv->def = calloc(num_blocks * size, sizeof(uint64_t));
    lv->live_in = calloc(num_blocks * size, sizeof(uint64_t));
    lv->live_out = calloc(num_blocks * size, sizeof(uint64_t));
    return lv;
}

int compute_live_vars(LiveVars *lv, int **cfg, int num_blocks, int size) {
    int changed = 1;
    int iterations = 0;
    while (changed) {
        changed = 0;
        iterations++;
        for (int b = num_blocks - 1; b >= 0; b--) {
            uint64_t old_in[size];
            memcpy(old_in, &lv->live_in[b * size], size * sizeof(uint64_t));
            
            for (int s = 0; s < num_blocks; s++) {
                if (cfg[b][s]) {
                    union_sets(&lv->live_out[b * size], &lv->live_in[s * size], size);
                }
            }
            
            memcpy(&lv->live_in[b * size], &lv->live_out[b * size], size * sizeof(uint64_t));
            for (int i = 0; i < size; i++) {
                lv->live_in[b * size + i] = (lv->live_out[b * size + i] & ~lv->def[b * size + i]) | lv->use[b * size + i];
            }
            
            for (int i = 0; i < size; i++) {
                if (old_in[i] != lv->live_in[b * size + i]) {
                    changed = 1;
                    break;
                }
            }
        }
    }
    return iterations;
}

/* Available Expressions Analysis */
typedef struct {
    uint64_t *gen;
    uint64_t *kill;
    uint64_t *avail_in;
    uint64_t *avail_out;
} AvailExprs;

AvailExprs* create_avail_exprs(int num_blocks, int num_exprs) {
    AvailExprs *ae = malloc(sizeof(AvailExprs));
    int size = (num_exprs + 63) / 64;
    ae->gen = calloc(num_blocks * size, sizeof(uint64_t));
    ae->kill = calloc(num_blocks * size, sizeof(uint64_t));
    ae->avail_in = calloc(num_blocks * size, sizeof(uint64_t));
    ae->avail_out = calloc(num_blocks * size, sizeof(uint64_t));
    return ae;
}

void intersect_sets(uint64_t *dest, uint64_t *src, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] &= src[i];
    }
}

int compute_available_exprs(AvailExprs *ae, int **cfg, int num_blocks, int size) {
    for (int b = 0; b < num_blocks; b++) {
        for (int i = 0; i < size; i++) {
            ae->avail_in[b * size + i] = ~0ULL;
        }
    }
    
    int changed = 1;
    int iterations = 0;
    while (changed) {
        changed = 0;
        iterations++;
        for (int b = 0; b < num_blocks; b++) {
            uint64_t old_out[size];
            memcpy(old_out, &ae->avail_out[b * size], size * sizeof(uint64_t));
            
            if (b > 0) {
                for (int i = 0; i < size; i++) {
                    ae->avail_in[b * size + i] = ~0ULL;
                }
                for (int p = 0; p < num_blocks; p++) {
                    if (cfg[p][b]) {
                        intersect_sets(&ae->avail_in[b * size], &ae->avail_out[p * size], size);
                    }
                }
            }
            
            for (int i = 0; i < size; i++) {
                ae->avail_out[b * size + i] = (ae->avail_in[b * size + i] & ~ae->kill[b * size + i]) | ae->gen[b * size + i];
            }
            
            for (int i = 0; i < size; i++) {
                if (old_out[i] != ae->avail_out[b * size + i]) {
                    changed = 1;
                    break;
                }
            }
        }
    }
    return iterations;
}

/* Very Busy Expressions Analysis */
typedef struct {
    uint64_t *use;
    uint64_t *def;
    uint64_t *busy_in;
    uint64_t *busy_out;
} BusyExprs;

BusyExprs* create_busy_exprs(int num_blocks, int num_exprs) {
    BusyExprs *be = malloc(sizeof(BusyExprs));
    int size = (num_exprs + 63) / 64;
    be->use = calloc(num_blocks * size, sizeof(uint64_t));
    be->def = calloc(num_blocks * size, sizeof(uint64_t));
    be->busy_in = calloc(num_blocks * size, sizeof(uint64_t));
    be->busy_out = calloc(num_blocks * size, sizeof(uint64_t));
    return be;
}

int compute_busy_exprs(BusyExprs *be, int **cfg, int num_blocks, int size) {
    int changed = 1;
    int iterations = 0;
    while (changed) {
        changed = 0;
        iterations++;
        for (int b = num_blocks - 1; b >= 0; b--) {
            uint64_t old_in[size];
            memcpy(old_in, &be->busy_in[b * size], size * sizeof(uint64_t));
            
            for (int i = 0; i < size; i++) {
                be->busy_out[b * size + i] = ~0ULL;
            }
            for (int s = 0; s < num_blocks; s++) {
                if (cfg[b][s]) {
                    intersect_sets(&be->busy_out[b * size], &be->busy_in[s * size], size);
                }
            }
            
            for (int i = 0; i < size; i++) {
                be->busy_in[b * size + i] = (be->busy_out[b * size + i] & ~be->def[b * size + i]) | be->use[b * size + i];
            }
            
            for (int i = 0; i < size; i++) {
                if (old_in[i] != be->busy_in[b * size + i]) {
                    changed = 1;
                    break;
                }
            }
        }
    }
    return iterations;
}

/* Constant Propagation Analysis */
typedef enum { UNDEF, CONST, NAC } LatticeValue;

typedef struct {
    LatticeValue type;
    int64_t value;
} ConstValue;

typedef struct {
    ConstValue **values;
    int num_vars;
} ConstProp;

ConstProp* create_const_prop(int num_blocks, int num_vars) {
    ConstProp *cp = malloc(sizeof(ConstProp));
    cp->num_vars = num_vars;
    cp->values = malloc(num_blocks * sizeof(ConstValue*));
    for (int i = 0; i < num_blocks; i++) {
        cp->values[i] = calloc(num_vars, sizeof(ConstValue));
    }
    return cp;
}

ConstValue meet_values(ConstValue v1, ConstValue v2) {
    if (v1.type == UNDEF) return v2;
    if (v2.type == UNDEF) return v1;
    if (v1.type == NAC || v2.type == NAC) return (ConstValue){NAC, 0};
    if (v1.value == v2.value) return v1;
    return (ConstValue){NAC, 0};
}

int compute_const_prop(ConstProp *cp, int **cfg, int num_blocks) {
    int changed = 1;
    int iterations = 0;
    while (changed) {
        changed = 0;
        iterations++;
        for (int b = 0; b < num_blocks; b++) {
            for (int v = 0; v < cp->num_vars; v++) {
                ConstValue old = cp->values[b][v];
                ConstValue new = {UNDEF, 0};
                
                for (int p = 0; p < num_blocks; p++) {
                    if (cfg[p][b]) {
                        new = meet_values(new, cp->values[p][v]);
                    }
                }
                
                cp->values[b][v] = new;
                if (old.type != new.type || old.value != new.value) {
                    changed = 1;
                }
            }
        }
    }
    return iterations;
}

/* Pointer Analysis - Andersen's Algorithm */
typedef struct PtrNode {
    int var;
    struct PtrNode *next;
} PtrNode;

typedef struct {
    PtrNode **points_to;
    int num_vars;
} PtrAnalysis;

PtrAnalysis* create_ptr_analysis(int num_vars) {
    PtrAnalysis *pa = malloc(sizeof(PtrAnalysis));
    pa->num_vars = num_vars;
    pa->points_to = calloc(num_vars, sizeof(PtrNode*));
    return pa;
}

void add_points_to(PtrAnalysis *pa, int ptr, int target) {
    PtrNode *node = malloc(sizeof(PtrNode));
    node->var = target;
    node->next = pa->points_to[ptr];
    pa->points_to[ptr] = node;
}

int has_points_to(PtrAnalysis *pa, int ptr, int target) {
    for (PtrNode *n = pa->points_to[ptr]; n; n = n->next) {
        if (n->var == target) return 1;
    }
    return 0;
}

int compute_andersen(PtrAnalysis *pa, int **constraints, int num_constraints) {
    int changed = 1;
    int iterations = 0;
    while (changed) {
        changed = 0;
        iterations++;
        for (int i = 0; i < num_constraints; i++) {
            int lhs = constraints[i][0];
            int rhs = constraints[i][1];
            int type = constraints[i][2];
            
            if (type == 0) {
                if (!has_points_to(pa, lhs, rhs)) {
                    add_points_to(pa, lhs, rhs);
                    changed = 1;
                }
            } else if (type == 1) {
                for (PtrNode *n = pa->points_to[rhs]; n; n = n->next) {
                    if (!has_points_to(pa, lhs, n->var)) {
                        add_points_to(pa, lhs, n->var);
                        changed = 1;
                    }
                }
            }
        }
    }
    return iterations;
}

/* Alias Analysis */
typedef struct {
    int **may_alias;
    int **must_alias;
    int num_vars;
} AliasAnalysis;

AliasAnalysis* create_alias_analysis(int num_vars) {
    AliasAnalysis *aa = malloc(sizeof(AliasAnalysis));
    aa->num_vars = num_vars;
    aa->may_alias = malloc(num_vars * sizeof(int*));
    aa->must_alias = malloc(num_vars * sizeof(int*));
    for (int i = 0; i < num_vars; i++) {
        aa->may_alias[i] = calloc(num_vars, sizeof(int));
        aa->must_alias[i] = calloc(num_vars, sizeof(int));
        aa->must_alias[i][i] = 1;
    }
    return aa;
}

void compute_may_alias(AliasAnalysis *aa, PtrAnalysis *pa) {
    for (int i = 0; i < aa->num_vars; i++) {
        for (int j = i; j < aa->num_vars; j++) {
            for (PtrNode *ni = pa->points_to[i]; ni; ni = ni->next) {
                for (PtrNode *nj = pa->points_to[j]; nj; nj = nj->next) {
                    if (ni->var == nj->var) {
                        aa->may_alias[i][j] = aa->may_alias[j][i] = 1;
                    }
                }
            }
        }
    }
}

/* Dominance Analysis */
typedef struct {
    uint64_t **dom;
    int *idom;
    int num_blocks;
    int size;
} DomAnalysis;

DomAnalysis* create_dom_analysis(int num_blocks) {
    DomAnalysis *da = malloc(sizeof(DomAnalysis));
    da->num_blocks = num_blocks;
    da->size = (num_blocks + 63) / 64;
    da->dom = malloc(num_blocks * sizeof(uint64_t*));
    da->idom = malloc(num_blocks * sizeof(int));
    for (int i = 0; i < num_blocks; i++) {
        da->dom[i] = malloc(da->size * sizeof(uint64_t));
        for (int j = 0; j < da->size; j++) {
            da->dom[i][j] = ~0ULL;
        }
        da->idom[i] = -1;
    }
    return da;
}

int compute_dominators(DomAnalysis *da, int **cfg) {
    set_bit(da->dom[0], 0);
    for (int i = 1; i < da->num_blocks; i++) {
        for (int j = 0; j < da->size; j++) {
            da->dom[i][j] = ~0ULL;
        }
    }
    
    int changed = 1;
    int iterations = 0;
    while (changed) {
        changed = 0;
        iterations++;
        for (int b = 1; b < da->num_blocks; b++) {
            uint64_t new_dom[da->size];
            for (int i = 0; i < da->size; i++) {
                new_dom[i] = ~0ULL;
            }
            
            for (int p = 0; p < da->num_blocks; p++) {
                if (cfg[p][b]) {
                    intersect_sets(new_dom, da->dom[p], da->size);
                }
            }
            set_bit(new_dom, b);
            
            for (int i = 0; i < da->size; i++) {
                if (new_dom[i] != da->dom[b][i]) {
                    changed = 1;
                    da->dom[b][i] = new_dom[i];
                }
            }
        }
    }
    
    for (int b = 0; b < da->num_blocks; b++) {
        for (int d = 0; d < da->num_blocks; d++) {
            if (d != b && test_bit(da->dom[b], d)) {
                int is_idom = 1;
                for (int x = 0; x < da->num_blocks; x++) {
                    if (x != b && x != d && test_bit(da->dom[b], x) && test_bit(da->dom[x], d)) {
                        is_idom = 0;
                        break;
                    }
                }
                if (is_idom) {
                    da->idom[b] = d;
                    break;
                }
            }
        }
    }
    
    return iterations;
}

/* Dominance Frontier */
typedef struct DFNode {
    int block;
    struct DFNode *next;
} DFNode;

typedef struct {
    DFNode **frontier;
    int num_blocks;
} DomFrontier;

DomFrontier* create_dom_frontier(int num_blocks) {
    DomFrontier *df = malloc(sizeof(DomFrontier));
    df->num_blocks = num_blocks;
    df->frontier = calloc(num_blocks, sizeof(DFNode*));
    return df;
}

void add_to_frontier(DomFrontier *df, int block, int frontier_block) {
    DFNode *node = malloc(sizeof(DFNode));
    node->block = frontier_block;
    node->next = df->frontier[block];
    df->frontier[block] = node;
}

void compute_dom_frontier(DomFrontier *df, DomAnalysis *da, int **cfg) {
    for (int b = 0; b < df->num_blocks; b++) {
        int pred_count = 0;
        for (int p = 0; p < df->num_blocks; p++) {
            if (cfg[p][b]) pred_count++;
        }
        
        if (pred_count >= 2) {
            for (int p = 0; p < df->num_blocks; p++) {
                if (cfg[p][b]) {
                    int runner = p;
                    while (runner != da->idom[b] && runner != -1) {
                        add_to_frontier(df, runner, b);
                        runner = da->idom[runner];
                    }
                }
            }
        }
    }
}

/* SSA Construction - Phi Placement */
typedef struct PhiNode {
    int var;
    int block;
    struct PhiNode *next;
} PhiNode;

typedef struct {
    PhiNode **phis;
    int num_blocks;
} SSAConstruct;

SSAConstruct* create_ssa_construct(int num_blocks) {
    SSAConstruct *ssa = malloc(sizeof(SSAConstruct));
    ssa->num_blocks = num_blocks;
    ssa->phis = calloc(num_blocks, sizeof(PhiNode*));
    return ssa;
}

void place_phi_functions(SSAConstruct *ssa, DomFrontier *df, int var, int *def_blocks, int num_defs) {
    int *has_phi = calloc(ssa->num_blocks, sizeof(int));
    int *worklist = malloc(num_defs * sizeof(int));
    int wl_size = 0;
    
    for (int i = 0; i < num_defs; i++) {
        worklist[wl_size++] = def_blocks[i];
    }
    
    while (wl_size > 0) {
        int block = worklist[--wl_size];
        for (DFNode *n = df->frontier[block]; n; n = n->next) {
            if (!has_phi[n->block]) {
                PhiNode *phi = malloc(sizeof(PhiNode));
                phi->var = var;
                phi->block = n->block;
                phi->next = ssa->phis[n->block];
                ssa->phis[n->block] = phi;
                has_phi[n->block] = 1;
                worklist[wl_size++] = n->block;
            }
        }
    }
    
    free(has_phi);
    free(worklist);
}

/* Loop Detection - Natural Loops */
typedef struct LoopNode {
    int header;
    int *blocks;
    int num_blocks;
    struct LoopNode *next;
} LoopNode;

typedef struct {
    LoopNode *loops;
    int num_loops;
} LoopInfo;

LoopInfo* create_loop_info() {
    LoopInfo *li = malloc(sizeof(LoopInfo));
    li->loops = NULL;
    li->num_loops = 0;
    return li;
}

int is_back_edge(int from, int to, DomAnalysis *da) {
    return test_bit(da->dom[from], to);
}

void find_natural_loop(LoopInfo *li, int header, int tail, int **cfg, int num_blocks) {
    int *in_loop = calloc(num_blocks, sizeof(int));
    int *stack = malloc(num_blocks * sizeof(int));
    int stack_size = 0;
    
    in_loop[header] = 1;
    if (tail != header) {
        in_loop[tail] = 1;
        stack[stack_size++] = tail;
    }
    
    while (stack_size > 0) {
        int block = stack[--stack_size];
        for (int p = 0; p < num_blocks; p++) {
            if (cfg[p][block] && !in_loop[p]) {
                in_loop[p] = 1;
                stack[stack_size++] = p;
            }
        }
    }
    
    LoopNode *loop = malloc(sizeof(LoopNode));
    loop->header = header;
    loop->num_blocks = 0;
    for (int i = 0; i < num_blocks; i++) {
        if (in_loop[i]) loop->num_blocks++;
    }
    loop->blocks = malloc(loop->num_blocks * sizeof(int));
    int idx = 0;
    for (int i = 0; i < num_blocks; i++) {
        if (in_loop[i]) loop->blocks[idx++] = i;
    }
    loop->next = li->loops;
    li->loops = loop;
    li->num_loops++;
    
    free(in_loop);
    free(stack);
}

void detect_loops(LoopInfo *li, DomAnalysis *da, int **cfg, int num_blocks) {
    for (int from = 0; from < num_blocks; from++) {
        for (int to = 0; to < num_blocks; to++) {
            if (cfg[from][to] && is_back_edge(from, to, da)) {
                find_natural_loop(li, to, from, cfg, num_blocks);
            }
        }
    }
}

/* Interval Analysis */

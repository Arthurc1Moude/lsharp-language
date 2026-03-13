/* Advanced Compiler Optimizations - Real Implementation */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Loop Invariant Code Motion */
typedef struct Instruction {
    int opcode;
    int dest;
    int src1;
    int src2;
    int is_invariant;
    struct Instruction *next;
} Instruction;

typedef struct BasicBlock {
    int id;
    Instruction *instructions;
    struct BasicBlock **successors;
    struct BasicBlock **predecessors;
    int num_succ;
    int num_pred;
    int in_loop;
} BasicBlock;

int is_loop_invariant(Instruction *inst, int *defs_in_loop, int num_vars) {
    if (inst->src1 >= 0 && defs_in_loop[inst->src1]) return 0;
    if (inst->src2 >= 0 && defs_in_loop[inst->src2]) return 0;
    return 1;
}

void hoist_invariants(BasicBlock *loop_header, BasicBlock **loop_blocks, int num_blocks, int num_vars) {
    int *defs_in_loop = calloc(num_vars, sizeof(int));
    int changed = 1;
    
    for (int i = 0; i < num_blocks; i++) {
        for (Instruction *inst = loop_blocks[i]->instructions; inst; inst = inst->next) {
            if (inst->dest >= 0) {
                defs_in_loop[inst->dest] = 1;
            }
        }
    }
    
    while (changed) {
        changed = 0;
        for (int i = 0; i < num_blocks; i++) {
            for (Instruction *inst = loop_blocks[i]->instructions; inst; inst = inst->next) {
                if (!inst->is_invariant && is_loop_invariant(inst, defs_in_loop, num_vars)) {
                    inst->is_invariant = 1;
                    if (inst->dest >= 0) {
                        defs_in_loop[inst->dest] = 0;
                    }
                    changed = 1;
                }
            }
        }
    }
    
    free(defs_in_loop);
}

/* Strength Reduction */
typedef struct InductionVar {
    int var;
    int basic_iv;
    int multiplier;
    int offset;
    struct InductionVar *next;
} InductionVar;

typedef struct {
    InductionVar *ivs;
    int num_ivs;
} IVInfo;

IVInfo* create_iv_info() {
    IVInfo *info = malloc(sizeof(IVInfo));
    info->ivs = NULL;
    info->num_ivs = 0;
    return info;
}

void add_induction_var(IVInfo *info, int var, int basic_iv, int mult, int off) {
    InductionVar *iv = malloc(sizeof(InductionVar));
    iv->var = var;
    iv->basic_iv = basic_iv;
    iv->multiplier = mult;
    iv->offset = off;
    iv->next = info->ivs;
    info->ivs = iv;
    info->num_ivs++;
}

void detect_induction_vars(IVInfo *info, BasicBlock **loop_blocks, int num_blocks) {
    for (int i = 0; i < num_blocks; i++) {
        for (Instruction *inst = loop_blocks[i]->instructions; inst; inst = inst->next) {
            if (inst->opcode == 0) {
                int is_basic = 1;
                for (InductionVar *iv = info->ivs; iv; iv = iv->next) {
                    if (iv->var == inst->src1 || iv->var == inst->src2) {
                        is_basic = 0;
                        break;
                    }
                }
                if (is_basic) {
                    add_induction_var(info, inst->dest, inst->dest, 1, 0);
                }
            } else if (inst->opcode == 1) {
                for (InductionVar *iv = info->ivs; iv; iv = iv->next) {
                    if (iv->var == inst->src1) {
                        add_induction_var(info, inst->dest, iv->basic_iv, iv->multiplier * inst->src2, iv->offset);
                    }
                }
            }
        }
    }
}

void apply_strength_reduction(BasicBlock **loop_blocks, int num_blocks, IVInfo *info) {
    for (InductionVar *iv = info->ivs; iv; iv = iv->next) {
        if (iv->multiplier != 1 || iv->offset != 0) {
            for (int i = 0; i < num_blocks; i++) {
                for (Instruction *inst = loop_blocks[i]->instructions; inst; inst = inst->next) {
                    if (inst->opcode == 1 && inst->src1 == iv->var) {
                        inst->opcode = 0;
                        inst->src2 = iv->multiplier;
                    }
                }
            }
        }
    }
}

/* Loop Unrolling */
typedef struct {
    BasicBlock **blocks;
    int num_blocks;
    int unroll_factor;
} UnrollInfo;

UnrollInfo* create_unroll_info(int factor) {
    UnrollInfo *ui = malloc(sizeof(UnrollInfo));
    ui->unroll_factor = factor;
    ui->blocks = NULL;
    ui->num_blocks = 0;
    return ui;
}

BasicBlock* duplicate_block(BasicBlock *orig, int iteration) {
    BasicBlock *new_block = malloc(sizeof(BasicBlock));
    new_block->id = orig->id * 1000 + iteration;
    new_block->instructions = NULL;
    
    Instruction *last = NULL;
    for (Instruction *inst = orig->instructions; inst; inst = inst->next) {
        Instruction *new_inst = malloc(sizeof(Instruction));
        memcpy(new_inst, inst, sizeof(Instruction));
        new_inst->next = NULL;
        
        if (last) last->next = new_inst;
        else new_block->instructions = new_inst;
        last = new_inst;
    }
    
    return new_block;
}

void unroll_loop(BasicBlock *header, BasicBlock **loop_blocks, int num_blocks, int factor) {
    BasicBlock **unrolled = malloc(num_blocks * factor * sizeof(BasicBlock*));
    
    for (int i = 0; i < factor; i++) {
        for (int j = 0; j < num_blocks; j++) {
            unrolled[i * num_blocks + j] = duplicate_block(loop_blocks[j], i);
        }
    }
    
    for (int i = 0; i < factor - 1; i++) {
        BasicBlock *tail = unrolled[(i + 1) * num_blocks - 1];
        BasicBlock *next_head = unrolled[(i + 1) * num_blocks];
        tail->num_succ = 1;
        tail->successors = malloc(sizeof(BasicBlock*));
        tail->successors[0] = next_head;
    }
    
    free(unrolled);
}

/* Loop Fusion */
typedef struct {
    BasicBlock *loop1_header;
    BasicBlock *loop2_header;
    int can_fuse;
} FusionCandidate;

int check_dependencies(BasicBlock **loop1, int n1, BasicBlock **loop2, int n2) {
    for (int i = 0; i < n1; i++) {
        for (Instruction *inst1 = loop1[i]->instructions; inst1; inst1 = inst1->next) {
            for (int j = 0; j < n2; j++) {
                for (Instruction *inst2 = loop2[j]->instructions; inst2; inst2 = inst2->next) {
                    if (inst1->dest == inst2->src1 || inst1->dest == inst2->src2) {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

void fuse_loops(BasicBlock *loop1_header, BasicBlock *loop2_header, BasicBlock **loop1_blocks, int n1, BasicBlock **loop2_blocks, int n2) {
    if (!check_dependencies(loop1_blocks, n1, loop2_blocks, n2)) {
        return;
    }
    
    for (int i = 0; i < n1; i++) {
        Instruction *last = loop1_blocks[i]->instructions;
        while (last && last->next) last = last->next;
        
        if (i < n2 && last) {
            last->next = loop2_blocks[i]->instructions;
        }
    }
}

/* Vectorization - SIMD */
typedef struct {
    int *vectorizable;
    int vector_width;
    int num_instructions;
} VectorInfo;

VectorInfo* create_vector_info(int num_inst, int width) {
    VectorInfo *vi = malloc(sizeof(VectorInfo));
    vi->num_instructions = num_inst;
    vi->vector_width = width;
    vi->vectorizable = calloc(num_inst, sizeof(int));
    return vi;
}

int check_vectorizable(Instruction *inst, int vector_width) {
    if (inst->opcode >= 0 && inst->opcode <= 5) {
        return 1;
    }
    return 0;
}

void vectorize_loop(BasicBlock **loop_blocks, int num_blocks, int vector_width) {
    for (int i = 0; i < num_blocks; i++) {
        Instruction *prev = NULL;
        for (Instruction *inst = loop_blocks[i]->instructions; inst; ) {
            if (check_vectorizable(inst, vector_width)) {
                Instruction *vec_inst = malloc(sizeof(Instruction));
                vec_inst->opcode = inst->opcode + 100;
                vec_inst->dest = inst->dest;
                vec_inst->src1 = inst->src1;
                vec_inst->src2 = inst->src2;
                vec_inst->next = inst->next;
                
                if (prev) prev->next = vec_inst;
                else loop_blocks[i]->instructions = vec_inst;
                
                Instruction *old = inst;
                inst = inst->next;
                free(old);
            } else {
                prev = inst;
                inst = inst->next;
            }
        }
    }
}

/* Software Pipelining */
typedef struct Stage {
    Instruction **instructions;
    int num_instructions;
    int stage_num;
} Stage;

typedef struct {
    Stage *stages;
    int num_stages;
    int initiation_interval;
} SWPSchedule;

SWPSchedule* create_swp_schedule(int num_stages, int ii) {
    SWPSchedule *sched = malloc(sizeof(SWPSchedule));
    sched->num_stages = num_stages;
    sched->initiation_interval = ii;
    sched->stages = malloc(num_stages * sizeof(Stage));
    for (int i = 0; i < num_stages; i++) {
        sched->stages[i].instructions = NULL;
        sched->stages[i].num_instructions = 0;
        sched->stages[i].stage_num = i;
    }
    return sched;
}

int compute_min_ii(BasicBlock *loop_body, int *rec_deps, int num_deps) {
    int max_rec = 0;
    for (int i = 0; i < num_deps; i++) {
        if (rec_deps[i] > max_rec) {
            max_rec = rec_deps[i];
        }
    }
    
    int resource_ii = 0;
    for (Instruction *inst = loop_body->instructions; inst; inst = inst->next) {
        resource_ii++;
    }
    
    return (max_rec > resource_ii) ? max_rec : resource_ii;
}

void schedule_software_pipeline(SWPSchedule *sched, BasicBlock *loop_body) {
    int *scheduled = calloc(1000, sizeof(int));
    int time = 0;
    
    for (Instruction *inst = loop_body->instructions; inst; inst = inst->next) {
        int earliest = 0;
        if (inst->src1 >= 0 && scheduled[inst->src1]) {
            earliest = scheduled[inst->src1] + 1;
        }
        if (inst->src2 >= 0 && scheduled[inst->src2]) {
            int t = scheduled[inst->src2] + 1;
            if (t > earliest) earliest = t;
        }
        
        int stage = earliest / sched->initiation_interval;
        if (stage >= sched->num_stages) {
            sched->num_stages = stage + 1;
            sched->stages = realloc(sched->stages, sched->num_stages * sizeof(Stage));
        }
        
        scheduled[inst->dest] = earliest;
        time = earliest;
    }
    
    free(scheduled);
}

/* Partial Redundancy Elimination */
typedef struct {
    uint64_t *anticipated;
    uint64_t *available;
    uint64_t *postponable;
    uint64_t *used;
    uint64_t *earliest;
    uint64_t *latest;
    int num_blocks;
    int num_exprs;
    int size;
} PREInfo;

PREInfo* create_pre_info(int num_blocks, int num_exprs) {
    PREInfo *pre = malloc(sizeof(PREInfo));
    pre->num_blocks = num_blocks;
    pre->num_exprs = num_exprs;
    pre->size = (num_exprs + 63) / 64;
    
    pre->anticipated = calloc(num_blocks * pre->size, sizeof(uint64_t));
    pre->available = calloc(num_blocks * pre->size, sizeof(uint64_t));
    pre->postponable = calloc(num_blocks * pre->size, sizeof(uint64_t));
    pre->used = calloc(num_blocks * pre->size, sizeof(uint64_t));
    pre->earliest = calloc(num_blocks * pre->size, sizeof(uint64_t));
    pre->latest = calloc(num_blocks * pre->size, sizeof(uint64_t));
    
    return pre;
}

void compute_anticipated(PREInfo *pre, int **cfg, uint64_t *use, uint64_t *kill) {
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int b = pre->num_blocks - 1; b >= 0; b--) {
            uint64_t old[pre->size];
            memcpy(old, &pre->anticipated[b * pre->size], pre->size * sizeof(uint64_t));
            
            for (int i = 0; i < pre->size; i++) {
                pre->anticipated[b * pre->size + i] = 0;
            }
            
            for (int s = 0; s < pre->num_blocks; s++) {
                if (cfg[b][s]) {
                    if (pre->anticipated[b * pre->size] == 0) {
                        memcpy(&pre->anticipated[b * pre->size], &pre->anticipated[s * pre->size], pre->size * sizeof(uint64_t));
                    } else {
                        for (int i = 0; i < pre->size; i++) {
                            pre->anticipated[b * pre->size + i] &= pre->anticipated[s * pre->size + i];
                        }
                    }
                }
            }
            
            for (int i = 0; i < pre->size; i++) {
                pre->anticipated[b * pre->size + i] = (pre->anticipated[b * pre->size + i] & ~kill[b * pre->size + i]) | use[b * pre->size + i];
            }
            
            for (int i = 0; i < pre->size; i++) {
                if (old[i] != pre->anticipated[b * pre->size + i]) {
                    changed = 1;
                    break;
                }
            }
        }
    }
}

/* Global Value Numbering */

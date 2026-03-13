/* ============================================================================
 * L# Compiler - Register Allocator
 * ============================================================================
 * File: regalloc.c
 * Purpose: Graph coloring register allocation with spilling
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_REGISTERS 32
#define MAX_VARIABLES 1024
#define MAX_INTERFERENCE_EDGES 10000

/* ============================================================================
 * DATA STRUCTURES
 * ============================================================================
 */

typedef struct LiveRange {
    int start;
    int end;
    int variable_id;
    int register_assigned;
    bool spilled;
} LiveRange;

typedef struct InterferenceEdge {
    int var1;
    int var2;
} InterferenceEdge;

typedef struct InterferenceGraph {
    int **adjacency_matrix;
    int node_count;
    InterferenceEdge *edges;
    int edge_count;
    int *degrees;
} InterferenceGraph;

typedef struct RegisterAllocator {
    LiveRange *live_ranges;
    int live_range_count;
    InterferenceGraph *interference_graph;
    int *register_map;
    int *spill_locations;
    int spill_count;
    int available_registers;
} RegisterAllocator;

static RegisterAllocator allocator;

/* ============================================================================
 * LIVENESS ANALYSIS
 * ============================================================================
 */

typedef struct LivenessInfo {
    int *live_in;
    int *live_out;
    int *use;
    int *def;
    int instruction_count;
} LivenessInfo;

LivenessInfo* create_liveness_info(int inst_count) {
    LivenessInfo *info = malloc(sizeof(LivenessInfo));
    info->instruction_count = inst_count;
    info->live_in = calloc(inst_count * MAX_VARIABLES, sizeof(int));
    info->live_out = calloc(inst_count * MAX_VARIABLES, sizeof(int));
    info->use = calloc(inst_count * MAX_VARIABLES, sizeof(int));
    info->def = calloc(inst_count * MAX_VARIABLES, sizeof(int));
    return info;
}

void compute_liveness(LivenessInfo *info) {
    bool changed = true;
    
    while (changed) {
        changed = false;
        
        for (int i = info->instruction_count - 1; i >= 0; i--) {
            int old_live_in[MAX_VARIABLES];
            memcpy(old_live_in, &info->live_in[i * MAX_VARIABLES], MAX_VARIABLES * sizeof(int));
            
            for (int v = 0; v < MAX_VARIABLES; v++) {
                info->live_in[i * MAX_VARIABLES + v] = 
                    info->use[i * MAX_VARIABLES + v] ||
                    (info->live_out[i * MAX_VARIABLES + v] && !info->def[i * MAX_VARIABLES + v]);
            }
            
            if (i < info->instruction_count - 1) {
                for (int v = 0; v < MAX_VARIABLES; v++) {
                    info->live_out[i * MAX_VARIABLES + v] = info->live_in[(i + 1) * MAX_VARIABLES + v];
                }
            }
            
            for (int v = 0; v < MAX_VARIABLES; v++) {
                if (old_live_in[v] != info->live_in[i * MAX_VARIABLES + v]) {
                    changed = true;
                    break;
                }
            }
        }
    }
}

void compute_live_ranges(LivenessInfo *info) {
    allocator.live_range_count = 0;
    
    for (int v = 0; v < MAX_VARIABLES; v++) {
        int first_use = -1;
        int last_use = -1;
        
        for (int i = 0; i < info->instruction_count; i++) {
            if (info->live_in[i * MAX_VARIABLES + v] || info->live_out[i * MAX_VARIABLES + v]) {
                if (first_use == -1) first_use = i;
                last_use = i;
            }
        }
        
        if (first_use != -1) {
            LiveRange *range = &allocator.live_ranges[allocator.live_range_count++];
            range->start = first_use;
            range->end = last_use;
            range->variable_id = v;
            range->register_assigned = -1;
            range->spilled = false;
        }
    }
}

/* ============================================================================
 * INTERFERENCE GRAPH CONSTRUCTION
 * ============================================================================
 */

InterferenceGraph* create_interference_graph(int node_count) {
    InterferenceGraph *graph = malloc(sizeof(InterferenceGraph));
    graph->node_count = node_count;
    graph->edge_count = 0;
    graph->edges = malloc(sizeof(InterferenceEdge) * MAX_INTERFERENCE_EDGES);
    graph->degrees = calloc(node_count, sizeof(int));
    
    graph->adjacency_matrix = malloc(sizeof(int*) * node_count);
    for (int i = 0; i < node_count; i++) {
        graph->adjacency_matrix[i] = calloc(node_count, sizeof(int));
    }
    
    return graph;
}

void add_interference_edge(InterferenceGraph *graph, int var1, int var2) {
    if (var1 == var2) return;
    if (graph->adjacency_matrix[var1][var2]) return;
    
    graph->adjacency_matrix[var1][var2] = 1;
    graph->adjacency_matrix[var2][var1] = 1;
    
    graph->edges[graph->edge_count].var1 = var1;
    graph->edges[graph->edge_count].var2 = var2;
    graph->edge_count++;
    
    graph->degrees[var1]++;
    graph->degrees[var2]++;
}

void build_interference_graph(LivenessInfo *info) {
    allocator.interference_graph = create_interference_graph(allocator.live_range_count);
    
    for (int i = 0; i < info->instruction_count; i++) {
        for (int v1 = 0; v1 < MAX_VARIABLES; v1++) {
            if (!info->live_out[i * MAX_VARIABLES + v1]) continue;
            
            for (int v2 = v1 + 1; v2 < MAX_VARIABLES; v2++) {
                if (!info->live_out[i * MAX_VARIABLES + v2]) continue;
                
                int range1 = -1, range2 = -1;
                for (int r = 0; r < allocator.live_range_count; r++) {
                    if (allocator.live_ranges[r].variable_id == v1) range1 = r;
                    if (allocator.live_ranges[r].variable_id == v2) range2 = r;
                }
                
                if (range1 != -1 && range2 != -1) {
                    add_interference_edge(allocator.interference_graph, range1, range2);
                }
            }
        }
    }
}

/* ============================================================================
 * GRAPH COLORING
 * ============================================================================
 */

typedef struct ColoringStack {
    int *nodes;
    int top;
    int capacity;
} ColoringStack;

ColoringStack* create_coloring_stack(int capacity) {
    ColoringStack *stack = malloc(sizeof(ColoringStack));
    stack->nodes = malloc(sizeof(int) * capacity);
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

void push_coloring_stack(ColoringStack *stack, int node) {
    if (stack->top < stack->capacity - 1) {
        stack->nodes[++stack->top] = node;
    }
}

int pop_coloring_stack(ColoringStack *stack) {
    if (stack->top >= 0) {
        return stack->nodes[stack->top--];
    }
    return -1;
}

bool is_coloring_stack_empty(ColoringStack *stack) {
    return stack->top < 0;
}

int find_min_degree_node(InterferenceGraph *graph, bool *removed) {
    int min_degree = MAX_VARIABLES + 1;
    int min_node = -1;
    
    for (int i = 0; i < graph->node_count; i++) {
        if (removed[i]) continue;
        
        int actual_degree = 0;
        for (int j = 0; j < graph->node_count; j++) {
            if (!removed[j] && graph->adjacency_matrix[i][j]) {
                actual_degree++;
            }
        }
        
        if (actual_degree < min_degree) {
            min_degree = actual_degree;
            min_node = i;
        }
    }
    
    return min_node;
}

void simplify_graph(InterferenceGraph *graph, ColoringStack *stack, bool *removed, int k) {
    bool progress = true;
    
    while (progress) {
        progress = false;
        
        for (int i = 0; i < graph->node_count; i++) {
            if (removed[i]) continue;
            
            int actual_degree = 0;
            for (int j = 0; j < graph->node_count; j++) {
                if (!removed[j] && graph->adjacency_matrix[i][j]) {
                    actual_degree++;
                }
            }
            
            if (actual_degree < k) {
                push_coloring_stack(stack, i);
                removed[i] = true;
                progress = true;
            }
        }
    }
}

int select_spill_candidate(InterferenceGraph *graph, bool *removed) {
    int max_degree = -1;
    int spill_node = -1;
    
    for (int i = 0; i < graph->node_count; i++) {
        if (removed[i]) continue;
        
        int actual_degree = 0;
        for (int j = 0; j < graph->node_count; j++) {
            if (!removed[j] && graph->adjacency_matrix[i][j]) {
                actual_degree++;
            }
        }
        
        if (actual_degree > max_degree) {
            max_degree = actual_degree;
            spill_node = i;
        }
    }
    
    return spill_node;
}

bool color_graph(InterferenceGraph *graph, int k) {
    ColoringStack *stack = create_coloring_stack(graph->node_count);
    bool *removed = calloc(graph->node_count, sizeof(bool));
    int *colors = malloc(sizeof(int) * graph->node_count);
    
    for (int i = 0; i < graph->node_count; i++) {
        colors[i] = -1;
    }
    
    while (true) {
        simplify_graph(graph, stack, removed, k);
        
        bool all_removed = true;
        for (int i = 0; i < graph->node_count; i++) {
            if (!removed[i]) {
                all_removed = false;
                break;
            }
        }
        
        if (all_removed) break;
        
        int spill_node = select_spill_candidate(graph, removed);
        if (spill_node == -1) break;
        
        push_coloring_stack(stack, spill_node);
        removed[spill_node] = true;
        allocator.live_ranges[spill_node].spilled = true;
        allocator.spill_count++;
    }
    
    while (!is_coloring_stack_empty(stack)) {
        int node = pop_coloring_stack(stack);
        
        bool used_colors[MAX_REGISTERS] = {false};
        
        for (int i = 0; i < graph->node_count; i++) {
            if (graph->adjacency_matrix[node][i] && colors[i] != -1) {
                used_colors[colors[i]] = true;
            }
        }
        
        int assigned_color = -1;
        for (int c = 0; c < k; c++) {
            if (!used_colors[c]) {
                assigned_color = c;
                break;
            }
        }
        
        if (assigned_color == -1) {
            allocator.live_ranges[node].spilled = true;
            allocator.spill_count++;
        } else {
            colors[node] = assigned_color;
            allocator.live_ranges[node].register_assigned = assigned_color;
        }
    }
    
    free(stack->nodes);
    free(stack);
    free(removed);
    free(colors);
    
    return allocator.spill_count == 0;
}

/* ============================================================================
 * SPILL CODE GENERATION
 * ============================================================================
 */

void generate_spill_code() {
    allocator.spill_locations = malloc(sizeof(int) * allocator.spill_count);
    int spill_offset = 0;
    
    for (int i = 0; i < allocator.live_range_count; i++) {
        if (allocator.live_ranges[i].spilled) {
            allocator.spill_locations[i] = spill_offset;
            spill_offset += 8;
        }
    }
}

void insert_spill_loads_stores() {
}

/* ============================================================================
 * COALESCING
 * ============================================================================
 */

bool can_coalesce(int var1, int var2, InterferenceGraph *graph) {
    if (graph->adjacency_matrix[var1][var2]) {
        return false;
    }
    
    int combined_degree = 0;
    bool neighbors[MAX_VARIABLES] = {false};
    
    for (int i = 0; i < graph->node_count; i++) {
        if (graph->adjacency_matrix[var1][i]) neighbors[i] = true;
        if (graph->adjacency_matrix[var2][i]) neighbors[i] = true;
    }
    
    for (int i = 0; i < graph->node_count; i++) {
        if (neighbors[i]) combined_degree++;
    }
    
    return combined_degree < allocator.available_registers;
}

void coalesce_moves(InterferenceGraph *graph) {
}

/* ============================================================================
 * REGISTER ALLOCATION MAIN ALGORITHM
 * ============================================================================
 */

void initialize_register_allocator(int num_registers) {
    allocator.live_ranges = malloc(sizeof(LiveRange) * MAX_VARIABLES);
    allocator.live_range_count = 0;
    allocator.register_map = malloc(sizeof(int) * MAX_VARIABLES);
    allocator.spill_count = 0;
    allocator.available_registers = num_registers;
    
    for (int i = 0; i < MAX_VARIABLES; i++) {
        allocator.register_map[i] = -1;
    }
}

bool allocate_registers(int instruction_count, int num_registers) {
    initialize_register_allocator(num_registers);
    
    LivenessInfo *liveness = create_liveness_info(instruction_count);
    compute_liveness(liveness);
    compute_live_ranges(liveness);
    build_interference_graph(liveness);
    
    bool success = color_graph(allocator.interference_graph, num_registers);
    
    if (!success) {
        generate_spill_code();
        insert_spill_loads_stores();
    }
    
    coalesce_moves(allocator.interference_graph);
    
    for (int i = 0; i < allocator.live_range_count; i++) {
        int var_id = allocator.live_ranges[i].variable_id;
        allocator.register_map[var_id] = allocator.live_ranges[i].register_assigned;
    }
    
    free(liveness->live_in);
    free(liveness->live_out);
    free(liveness->use);
    free(liveness->def);
    free(liveness);
    
    return success;
}

int get_assigned_register(int variable_id) {
    return allocator.register_map[variable_id];
}

bool is_variable_spilled(int variable_id) {
    for (int i = 0; i < allocator.live_range_count; i++) {
        if (allocator.live_ranges[i].variable_id == variable_id) {
            return allocator.live_ranges[i].spilled;
        }
    }
    return false;
}

int get_spill_location(int variable_id) {
    for (int i = 0; i < allocator.live_range_count; i++) {
        if (allocator.live_ranges[i].variable_id == variable_id) {
            return allocator.spill_locations[i];
        }
    }
    return -1;
}

/* ============================================================================
 * LINEAR SCAN REGISTER ALLOCATION
 * ============================================================================
 */

typedef struct ActiveInterval {
    int variable_id;
    int end_point;
    int register_assigned;
} ActiveInterval;

void linear_scan_allocation(int num_registers) {
    ActiveInterval *active = malloc(sizeof(ActiveInterval) * MAX_VARIABLES);
    int active_count = 0;
    
    for (int i = 0; i < allocator.live_range_count; i++) {
        LiveRange *current = &allocator.live_ranges[i];
        
        for (int j = 0; j < active_count; j++) {
            if (active[j].end_point < current->start) {
                for (int k = j; k < active_count - 1; k++) {
                    active[k] = active[k + 1];
                }
                active_count--;
                j--;
            }
        }
        
        if (active_count < num_registers) {
            bool used[MAX_REGISTERS] = {false};
            for (int j = 0; j < active_count; j++) {
                used[active[j].register_assigned] = true;
            }
            
            for (int r = 0; r < num_registers; r++) {
                if (!used[r]) {
                    current->register_assigned = r;
                    active[active_count].variable_id = current->variable_id;
                    active[active_count].end_point = current->end;
                    active[active_count].register_assigned = r;
                    active_count++;
                    break;
                }
            }
        } else {
            int spill_idx = 0;
            for (int j = 1; j < active_count; j++) {
                if (active[j].end_point > active[spill_idx].end_point) {
                    spill_idx = j;
                }
            }
            
            if (active[spill_idx].end_point > current->end) {
                current->register_assigned = active[spill_idx].register_assigned;
                allocator.live_ranges[active[spill_idx].variable_id].spilled = true;
                active[spill_idx].variable_id = current->variable_id;
                active[spill_idx].end_point = current->end;
            } else {
                current->spilled = true;
            }
        }
    }
    
    free(active);
}

/* ============================================================================
 * DEBUGGING AND STATISTICS
 * ============================================================================
 */

void print_live_ranges() {
    printf("\n=== Live Ranges ===\n");
    for (int i = 0; i < allocator.live_range_count; i++) {
        LiveRange *range = &allocator.live_ranges[i];
        printf("Var %d: [%d, %d] -> ", range->variable_id, range->start, range->end);
        if (range->spilled) {
            printf("SPILLED\n");
        } else {
            printf("R%d\n", range->register_assigned);
        }
    }
    printf("===================\n");
}

void print_interference_graph() {
    printf("\n=== Interference Graph ===\n");
    InterferenceGraph *graph = allocator.interference_graph;
    
    for (int i = 0; i < graph->node_count; i++) {
        printf("Node %d (degree %d): ", i, graph->degrees[i]);
        for (int j = 0; j < graph->node_count; j++) {
            if (graph->adjacency_matrix[i][j]) {
                printf("%d ", j);
            }
        }
        printf("\n");
    }
    printf("==========================\n");
}

void print_register_allocation_stats() {
    printf("\n=== Register Allocation Statistics ===\n");
    printf("Variables: %d\n", allocator.live_range_count);
    printf("Spills: %d\n", allocator.spill_count);
    printf("Available registers: %d\n", allocator.available_registers);
    printf("Interference edges: %d\n", allocator.interference_graph->edge_count);
    printf("======================================\n");
}

void cleanup_register_allocator() {
    if (allocator.live_ranges) free(allocator.live_ranges);
    if (allocator.register_map) free(allocator.register_map);
    if (allocator.spill_locations) free(allocator.spill_locations);
    
    if (allocator.interference_graph) {
        for (int i = 0; i < allocator.interference_graph->node_count; i++) {
            free(allocator.interference_graph->adjacency_matrix[i]);
        }
        free(allocator.interference_graph->adjacency_matrix);
        free(allocator.interference_graph->edges);
        free(allocator.interference_graph->degrees);
        free(allocator.interference_graph);
    }
}

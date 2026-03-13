/* Graph Algorithms for Compiler CFG Analysis */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Graph Structure */
typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;

typedef struct Graph {
    int num_vertices;
    Edge **adj_list;
    int **adj_matrix;
} Graph;

Graph* create_graph(int n) {
    Graph *g = malloc(sizeof(Graph));
    g->num_vertices = n;
    g->adj_list = calloc(n, sizeof(Edge*));
    g->adj_matrix = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        g->adj_matrix[i] = calloc(n, sizeof(int));
    }
    return g;
}

void add_edge(Graph *g, int src, int dest, int weight) {
    Edge *e = malloc(sizeof(Edge));
    e->dest = dest;
    e->weight = weight;
    e->next = g->adj_list[src];
    g->adj_list[src] = e;
    g->adj_matrix[src][dest] = weight;
}

/* Depth-First Search */
void dfs_util(Graph *g, int v, int *visited, int *stack, int *top) {
    visited[v] = 1;
    
    for (Edge *e = g->adj_list[v]; e; e = e->next) {
        if (!visited[e->dest]) {
            dfs_util(g, e->dest, visited, stack, top);
        }
    }
    
    stack[(*top)++] = v;
}

void dfs(Graph *g, int start, int *visited) {
    int *stack = malloc(g->num_vertices * sizeof(int));
    int top = 0;
    
    dfs_util(g, start, visited, stack, &top);
    
    free(stack);
}

/* Breadth-First Search */
void bfs(Graph *g, int start, int *visited, int *distance) {
    int *queue = malloc(g->num_vertices * sizeof(int));
    int front = 0, rear = 0;
    
    for (int i = 0; i < g->num_vertices; i++) {
        distance[i] = INT_MAX;
    }
    
    visited[start] = 1;
    distance[start] = 0;
    queue[rear++] = start;
    
    while (front < rear) {
        int v = queue[front++];
        
        for (Edge *e = g->adj_list[v]; e; e = e->next) {
            if (!visited[e->dest]) {
                visited[e->dest] = 1;
                distance[e->dest] = distance[v] + 1;
                queue[rear++] = e->dest;
            }
        }
    }
    
    free(queue);
}

/* Dijkstra's Shortest Path */
int min_distance(int *dist, int *visited, int n) {
    int min = INT_MAX, min_idx = -1;
    
    for (int v = 0; v < n; v++) {
        if (!visited[v] && dist[v] < min) {
            min = dist[v];
            min_idx = v;
        }
    }
    
    return min_idx;
}

void dijkstra(Graph *g, int src, int *dist) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    
    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INT_MAX;
    }
    dist[src] = 0;
    
    for (int count = 0; count < g->num_vertices - 1; count++) {
        int u = min_distance(dist, visited, g->num_vertices);
        
        if (u == -1) break;
        
        visited[u] = 1;
        
        for (Edge *e = g->adj_list[u]; e; e = e->next) {
            int v = e->dest;
            if (!visited[v] && dist[u] != INT_MAX && 
                dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
            }
        }
    }
    
    free(visited);
}

/* Bellman-Ford Algorithm */
int bellman_ford(Graph *g, int src, int *dist) {
    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INT_MAX;
    }
    dist[src] = 0;
    
    for (int i = 0; i < g->num_vertices - 1; i++) {
        for (int u = 0; u < g->num_vertices; u++) {
            for (Edge *e = g->adj_list[u]; e; e = e->next) {
                int v = e->dest;
                if (dist[u] != INT_MAX && dist[u] + e->weight < dist[v]) {
                    dist[v] = dist[u] + e->weight;
                }
            }
        }
    }
    
    for (int u = 0; u < g->num_vertices; u++) {
        for (Edge *e = g->adj_list[u]; e; e = e->next) {
            int v = e->dest;
            if (dist[u] != INT_MAX && dist[u] + e->weight < dist[v]) {
                return 0;
            }
        }
    }
    
    return 1;
}

/* Floyd-Warshall All-Pairs Shortest Path */
void floyd_warshall(Graph *g, int **dist) {
    for (int i = 0; i < g->num_vertices; i++) {
        for (int j = 0; j < g->num_vertices; j++) {
            if (i == j) {
                dist[i][j] = 0;
            } else if (g->adj_matrix[i][j]) {
                dist[i][j] = g->adj_matrix[i][j];
            } else {
                dist[i][j] = INT_MAX / 2;
            }
        }
    }
    
    for (int k = 0; k < g->num_vertices; k++) {
        for (int i = 0; i < g->num_vertices; i++) {
            for (int j = 0; j < g->num_vertices; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}

/* Topological Sort */
void topological_sort_util(Graph *g, int v, int *visited, int *stack, int *top) {
    visited[v] = 1;
    
    for (Edge *e = g->adj_list[v]; e; e = e->next) {
        if (!visited[e->dest]) {
            topological_sort_util(g, e->dest, visited, stack, top);
        }
    }
    
    stack[(*top)++] = v;
}

int* topological_sort(Graph *g) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    int *stack = malloc(g->num_vertices * sizeof(int));
    int top = 0;
    
    for (int i = 0; i < g->num_vertices; i++) {
        if (!visited[i]) {
            topological_sort_util(g, i, visited, stack, &top);
        }
    }
    
    int *result = malloc(g->num_vertices * sizeof(int));
    for (int i = 0; i < g->num_vertices; i++) {
        result[i] = stack[g->num_vertices - 1 - i];
    }
    
    free(visited);
    free(stack);
    return result;
}

/* Strongly Connected Components - Kosaraju's Algorithm */
Graph* transpose_graph(Graph *g) {
    Graph *gt = create_graph(g->num_vertices);
    
    for (int v = 0; v < g->num_vertices; v++) {
        for (Edge *e = g->adj_list[v]; e; e = e->next) {
            add_edge(gt, e->dest, v, e->weight);
        }
    }
    
    return gt;
}

void fill_order(Graph *g, int v, int *visited, int *stack, int *top) {
    visited[v] = 1;
    
    for (Edge *e = g->adj_list[v]; e; e = e->next) {
        if (!visited[e->dest]) {
            fill_order(g, e->dest, visited, stack, top);
        }
    }
    
    stack[(*top)++] = v;
}

void dfs_scc(Graph *g, int v, int *visited, int *component, int comp_id) {
    visited[v] = 1;
    component[v] = comp_id;
    
    for (Edge *e = g->adj_list[v]; e; e = e->next) {
        if (!visited[e->dest]) {
            dfs_scc(g, e->dest, visited, component, comp_id);
        }
    }
}

int kosaraju_scc(Graph *g, int *component) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    int *stack = malloc(g->num_vertices * sizeof(int));
    int top = 0;
    
    for (int i = 0; i < g->num_vertices; i++) {
        if (!visited[i]) {
            fill_order(g, i, visited, stack, &top);
        }
    }
    
    Graph *gt = transpose_graph(g);
    
    memset(visited, 0, g->num_vertices * sizeof(int));
    
    int comp_count = 0;
    for (int i = top - 1; i >= 0; i--) {
        int v = stack[i];
        if (!visited[v]) {
            dfs_scc(gt, v, visited, component, comp_count);
            comp_count++;
        }
    }
    
    free(visited);
    free(stack);
    
    for (int i = 0; i < gt->num_vertices; i++) {
        Edge *e = gt->adj_list[i];
        while (e) {
            Edge *tmp = e;
            e = e->next;
            free(tmp);
        }
        free(gt->adj_matrix[i]);
    }
    free(gt->adj_list);
    free(gt->adj_matrix);
    free(gt);
    
    return comp_count;
}

/* Tarjan's SCC Algorithm */
typedef struct {
    int *disc;
    int *low;
    int *on_stack;
    int *stack;
    int top;
    int time;
    int *component;
    int comp_count;
} TarjanData;

void tarjan_util(Graph *g, int u, TarjanData *data) {
    data->disc[u] = data->low[u] = data->time++;
    data->stack[data->top++] = u;
    data->on_stack[u] = 1;
    
    for (Edge *e = g->adj_list[u]; e; e = e->next) {
        int v = e->dest;
        
        if (data->disc[v] == -1) {
            tarjan_util(g, v, data);
            if (data->low[v] < data->low[u]) {
                data->low[u] = data->low[v];
            }
        } else if (data->on_stack[v]) {
            if (data->disc[v] < data->low[u]) {
                data->low[u] = data->disc[v];
            }
        }
    }
    
    if (data->low[u] == data->disc[u]) {
        while (1) {
            int w = data->stack[--data->top];
            data->on_stack[w] = 0;
            data->component[w] = data->comp_count;
            if (w == u) break;
        }
        data->comp_count++;
    }
}

int tarjan_scc(Graph *g, int *component) {
    TarjanData data;
    data.disc = malloc(g->num_vertices * sizeof(int));
    data.low = malloc(g->num_vertices * sizeof(int));
    data.on_stack = calloc(g->num_vertices, sizeof(int));
    data.stack = malloc(g->num_vertices * sizeof(int));
    data.top = 0;
    data.time = 0;
    data.component = component;
    data.comp_count = 0;
    
    for (int i = 0; i < g->num_vertices; i++) {
        data.disc[i] = data.low[i] = -1;
    }
    
    for (int i = 0; i < g->num_vertices; i++) {
        if (data.disc[i] == -1) {
            tarjan_util(g, i, &data);
        }
    }
    
    int count = data.comp_count;
    
    free(data.disc);
    free(data.low);
    free(data.on_stack);
    free(data.stack);
    
    return count;
}

/* Kruskal's Minimum Spanning Tree */
typedef struct {
    int src, dest, weight;
} WeightedEdge;

typedef struct {
    int *parent;
    int *rank;
} DisjointSet;

DisjointSet* create_disjoint_set(int n) {
    DisjointSet *ds = malloc(sizeof(DisjointSet));
    ds->parent = malloc(n * sizeof(int));
    ds->rank = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        ds->parent[i] = i;
    }
    return ds;
}

int find_set(DisjointSet *ds, int x) {
    if (ds->parent[x] != x) {
        ds->parent[x] = find_set(ds, ds->parent[x]);
    }
    return ds->parent[x];
}

void union_sets(DisjointSet *ds, int x, int y) {
    int px = find_set(ds, x);
    int py = find_set(ds, y);
    
    if (px == py) return;
    
    if (ds->rank[px] < ds->rank[py]) {
        ds->parent[px] = py;
    } else if (ds->rank[px] > ds->rank[py]) {
        ds->parent[py] = px;
    } else {
        ds->parent[py] = px;
        ds->rank[px]++;
    }
}

int compare_edges(const void *a, const void *b) {
    return ((WeightedEdge*)a)->weight - ((WeightedEdge*)b)->weight;
}

WeightedEdge* kruskal_mst(Graph *g, int *mst_size) {
    WeightedEdge *edges = malloc(g->num_vertices * g->num_vertices * sizeof(WeightedEdge));
    int edge_count = 0;
    
    for (int u = 0; u < g->num_vertices; u++) {
        for (Edge *e = g->adj_list[u]; e; e = e->next) {
            edges[edge_count].src = u;
            edges[edge_count].dest = e->dest;
            edges[edge_count].weight = e->weight;
            edge_count++;
        }
    }
    
    qsort(edges, edge_count, sizeof(WeightedEdge), compare_edges);
    
    DisjointSet *ds = create_disjoint_set(g->num_vertices);
    WeightedEdge *mst = malloc((g->num_vertices - 1) * sizeof(WeightedEdge));
    *mst_size = 0;
    
    for (int i = 0; i < edge_count && *mst_size < g->num_vertices - 1; i++) {
        int u = edges[i].src;
        int v = edges[i].dest;
        
        if (find_set(ds, u) != find_set(ds, v)) {
            mst[*mst_size] = edges[i];
            (*mst_size)++;
            union_sets(ds, u, v);
        }
    }
    
    free(edges);
    free(ds->parent);
    free(ds->rank);
    free(ds);
    
    return mst;
}

/* Prim's Minimum Spanning Tree */
WeightedEdge* prim_mst(Graph *g, int *mst_size) {
    int *key = malloc(g->num_vertices * sizeof(int));
    int *parent = malloc(g->num_vertices * sizeof(int));
    int *in_mst = calloc(g->num_vertices, sizeof(int));
    
    for (int i = 0; i < g->num_vertices; i++) {
        key[i] = INT_MAX;
        parent[i] = -1;
    }
    
    key[0] = 0;
    
    for (int count = 0; count < g->num_vertices; count++) {
        int u = -1, min = INT_MAX;
        
        for (int v = 0; v < g->num_vertices; v++) {
            if (!in_mst[v] && key[v] < min) {
                min = key[v];
                u = v;
            }
        }
        
        if (u == -1) break;
        
        in_mst[u] = 1;
        
        for (Edge *e = g->adj_list[u]; e; e = e->next) {
            int v = e->dest;
            if (!in_mst[v] && e->weight < key[v]) {
                key[v] = e->weight;
                parent[v] = u;
            }
        }
    }
    
    WeightedEdge *mst = malloc((g->num_vertices - 1) * sizeof(WeightedEdge));
    *mst_size = 0;
    
    for (int i = 1; i < g->num_vertices; i++) {
        if (parent[i] != -1) {
            mst[*mst_size].src = parent[i];
            mst[*mst_size].dest = i;
            mst[*mst_size].weight = key[i];
            (*mst_size)++;
        }
    }
    
    free(key);
    free(parent);
    free(in_mst);
    
    return mst;
}

/* Articulation Points and Bridges */
typedef struct {
    int *disc;
    int *low;
    int *parent;
    int *ap;
    int time;
} APData;

void find_ap_util(Graph *g, int u, APData *data) {
    int children = 0;
    data->disc[u] = data->low[u] = ++data->time;
    
    for (Edge *e = g->adj_list[u]; e; e = e->next) {
        int v = e->dest;
        
        if (data->disc[v] == -1) {
            children++;
            data->parent[v] = u;
            find_ap_util(g, v, data);
            
            if (data->low[v] < data->low[u]) {
                data->low[u] = data->low[v];
            }
            
            if (data->parent[u] == -1 && children > 1) {
                data->ap[u] = 1;
            }
            
            if (data->parent[u] != -1 && data->low[v] >= data->disc[u]) {
                data->ap[u] = 1;
            }
        } else if (v != data->parent[u]) {
            if (data->disc[v] < data->low[u]) {
                data->low[u] = data->disc[v];
            }
        }
    }
}

int* find_articulation_points(Graph *g, int *count) {
    APData data;
    data.disc = malloc(g->num_vertices * sizeof(int));
    data.low = malloc(g->num_vertices * sizeof(int));
    data.parent = malloc(g->num_vertices * sizeof(int));
    data.ap = calloc(g->num_vertices, sizeof(int));
    data.time = 0;
    
    for (int i = 0; i < g->num_vertices; i++) {
        data.disc[i] = data.low[i] = -1;
        data.parent[i] = -1;
    }
    
    for (int i = 0; i < g->num_vertices; i++) {
        if (data.disc[i] == -1) {
            find_ap_util(g, i, &data);
        }
    }
    
    *count = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (data.ap[i]) (*count)++;
    }
    
    int *result = malloc(*count * sizeof(int));
    int idx = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (data.ap[i]) {
            result[idx++] = i;
        }
    }
    
    free(data.disc);
    free(data.low);
    free(data.parent);
    free(data.ap);
    
    return result;
}

/* Maximum Flow - Ford-Fulkerson with BFS (Edmonds-Karp) */
int bfs_max_flow(int **residual, int s, int t, int n, int *parent) {
    int *visited = calloc(n, sizeof(int));
    int *queue = malloc(n * sizeof(int));
    int front = 0, rear = 0;
    
    queue[rear++] = s;
    visited[s] = 1;
    parent[s] = -1;
    
    while (front < rear) {
        int u = queue[front++];
        
        for (int v = 0; v < n; v++) {
            if (!visited[v] && residual[u][v] > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = 1;
                if (v == t) {
                    free(visited);
                    free(queue);
                    return 1;
                }
            }
        }
    }
    
    free(visited);
    free(queue);
    return 0;
}

int ford_fulkerson(Graph *g, int source, int sink) {
    int **residual = malloc(g->num_vertices * sizeof(int*));
    for (int i = 0; i < g->num_vertices; i++) {
        residual[i] = malloc(g->num_vertices * sizeof(int));
        for (int j = 0; j < g->num_vertices; j++) {
            residual[i][j] = g->adj_matrix[i][j];
        }
    }
    
    int *parent = malloc(g->num_vertices * sizeof(int));
    int max_flow = 0;
    
    while (bfs_max_flow(residual, source, sink, g->num_vertices, parent)) {
        int path_flow = INT_MAX;
        
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            if (residual[u][v] < path_flow) {
                path_flow = residual[u][v];
            }
        }
        
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residual[u][v] -= path_flow;
            residual[v][u] += path_flow;
        }
        
        max_flow += path_flow;
    }
    
    for (int i = 0; i < g->num_vertices; i++) {
        free(residual[i]);
    }
    free(residual);
    free(parent);
    
    return max_flow;
}

/* Bipartite Matching - Hungarian Algorithm */
int bpm_util(Graph *g, int u, int *match_r, int *visited) {
    for (Edge *e = g->adj_list[u]; e; e = e->next) {
        int v = e->dest;
        
        if (visited[v]) continue;
        
        visited[v] = 1;
        
        if (match_r[v] == -1 || bpm_util(g, match_r[v], match_r, visited)) {
            match_r[v] = u;
            return 1;
        }
    }
    return 0;
}

int max_bipartite_matching(Graph *g, int m, int n) {
    int *match_r = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        match_r[i] = -1;
    }
    
    int result = 0;
    for (int u = 0; u < m; u++) {
        int *visited = calloc(n, sizeof(int));
        if (bpm_util(g, u, match_r, visited)) {
            result++;
        }
        free(visited);
    }
    
    free(match_r);
    return result;
}

/* Graph Coloring - Greedy */
int* graph_coloring(Graph *g) {
    int *result = malloc(g->num_vertices * sizeof(int));
    int *available = malloc(g->num_vertices * sizeof(int));
    
    for (int i = 0; i < g->num_vertices; i++) {
        result[i] = -1;
    }
    
    result[0] = 0;
    
    for (int u = 1; u < g->num_vertices; u++) {
        for (int i = 0; i < g->num_vertices; i++) {
            available[i] = 1;
        }
        
        for (Edge *e = g->adj_list[u]; e; e = e->next) {
            if (result[e->dest] != -1) {
                available[result[e->dest]] = 0;
            }
        }
        
        int color;
        for (color = 0; color < g->num_vertices; color++) {
            if (available[color]) break;
        }
        
        result[u] = color;
    }
    
    free(available);
    return result;
}

/* Cycle Detection */
int is_cyclic_util(Graph *g, int v, int *visited, int *rec_stack) {
    visited[v] = 1;
    rec_stack[v] = 1;
    
    for (Edge *e = g->adj_list[v]; e; e = e->next) {
        if (!visited[e->dest]) {
            if (is_cyclic_util(g, e->dest, visited, rec_stack)) {
                return 1;
            }
        } else if (rec_stack[e->dest]) {
            return 1;
        }
    }
    
    rec_stack[v] = 0;
    return 0;
}

int is_cyclic(Graph *g) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    int *rec_stack = calloc(g->num_vertices, sizeof(int));
    
    for (int i = 0; i < g->num_vertices; i++) {
        if (!visited[i]) {
            if (is_cyclic_util(g, i, visited, rec_stack)) {
                free(visited);
                free(rec_stack);
                return 1;
            }
        }
    }
    
    free(visited);
    free(rec_stack);
    return 0;
}

/* Hamiltonian Path */
int is_safe(Graph *g, int v, int *path, int pos) {
    int found = 0;
    for (Edge *e = g->adj_list[path[pos - 1]]; e; e = e->next) {
        if (e->dest == v) {
            found = 1;
            break;
        }
    }
    if (!found) return 0;
    
    for (int i = 0; i < pos; i++) {
        if (path[i] == v) return 0;
    }
    
    return 1;
}

int hamiltonian_util(Graph *g, int *path, int pos) {
    if (pos == g->num_vertices) {
        return 1;
    }
    
    for (int v = 1; v < g->num_vertices; v++) {
        if (is_safe(g, v, path, pos)) {
            path[pos] = v;
            
            if (hamiltonian_util(g, path, pos + 1)) {
                return 1;
            }
            
            path[pos] = -1;
        }
    }
    
    return 0;
}

int* find_hamiltonian_path(Graph *g) {
    int *path = malloc(g->num_vertices * sizeof(int));
    for (int i = 0; i < g->num_vertices; i++) {
        path[i] = -1;
    }
    
    path[0] = 0;
    
    if (!hamiltonian_util(g, path, 1)) {
        free(path);
        return NULL;
    }
    
    return path;
}

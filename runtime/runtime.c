/* ============================================================================
 * L# Programming Language - Runtime Library Implementation
 * ============================================================================
 * File: runtime.c
 * Purpose: Core runtime functions, memory management, and standard library
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* ============================================================================
 * MEMORY MANAGEMENT
 * ============================================================================
 */

typedef struct MemoryBlock {
    void *ptr;
    size_t size;
    int ref_count;
    struct MemoryBlock *next;
} MemoryBlock;

static MemoryBlock *memory_blocks = NULL;
static size_t total_allocated = 0;
static size_t total_freed = 0;
static size_t allocation_count = 0;

void* ls_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Fatal: Out of memory\n");
        exit(1);
    }
    
    MemoryBlock *block = malloc(sizeof(MemoryBlock));
    block->ptr = ptr;
    block->size = size;
    block->ref_count = 1;
    block->next = memory_blocks;
    memory_blocks = block;
    
    total_allocated += size;
    allocation_count++;
    
    return ptr;
}

void* ls_realloc(void *ptr, size_t new_size) {
    MemoryBlock *block = memory_blocks;
    while (block) {
        if (block->ptr == ptr) {
            void *new_ptr = realloc(ptr, new_size);
            if (!new_ptr) {
                fprintf(stderr, "Fatal: Out of memory\n");
                exit(1);
            }
            
            total_allocated += (new_size - block->size);
            block->ptr = new_ptr;
            block->size = new_size;
            
            return new_ptr;
        }
        block = block->next;
    }
    
    return ls_malloc(new_size);
}

void ls_free(void *ptr) {
    if (!ptr) return;
    
    MemoryBlock **block_ptr = &memory_blocks;
    while (*block_ptr) {
        if ((*block_ptr)->ptr == ptr) {
            MemoryBlock *block = *block_ptr;
            block->ref_count--;
            
            if (block->ref_count <= 0) {
                *block_ptr = block->next;
                total_freed += block->size;
                free(block->ptr);
                free(block);
            }
            return;
        }
        block_ptr = &(*block_ptr)->next;
    }
}

void ls_incref(void *ptr) {
    MemoryBlock *block = memory_blocks;
    while (block) {
        if (block->ptr == ptr) {
            block->ref_count++;
            return;
        }
        block = block->next;
    }
}

void ls_decref(void *ptr) {
    ls_free(ptr);
}

void ls_print_memory_stats() {
    printf("\n=== Memory Statistics ===\n");
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Total freed: %zu bytes\n", total_freed);
    printf("Currently in use: %zu bytes\n", total_allocated - total_freed);
    printf("Allocation count: %zu\n", allocation_count);
    printf("========================\n");
}

/* ============================================================================
 * STRING OPERATIONS
 * ============================================================================
 */

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
    int ref_count;
} LSString;

LSString* ls_string_new(const char *str) {
    LSString *s = ls_malloc(sizeof(LSString));
    s->length = strlen(str);
    s->capacity = s->length + 1;
    s->data = ls_malloc(s->capacity);
    strcpy(s->data, str);
    s->ref_count = 1;
    return s;
}

LSString* ls_string_concat(LSString *s1, LSString *s2) {
    size_t new_len = s1->length + s2->length;
    LSString *result = ls_malloc(sizeof(LSString));
    result->length = new_len;
    result->capacity = new_len + 1;
    result->data = ls_malloc(result->capacity);
    strcpy(result->data, s1->data);
    strcat(result->data, s2->data);
    result->ref_count = 1;
    return result;
}

LSString* ls_string_substring(LSString *s, size_t start, size_t length) {
    if (start >= s->length) return ls_string_new("");
    if (start + length > s->length) length = s->length - start;
    
    LSString *result = ls_malloc(sizeof(LSString));
    result->length = length;
    result->capacity = length + 1;
    result->data = ls_malloc(result->capacity);
    strncpy(result->data, s->data + start, length);
    result->data[length] = '\0';
    result->ref_count = 1;
    return result;
}

int ls_string_compare(LSString *s1, LSString *s2) {
    return strcmp(s1->data, s2->data);
}

bool ls_string_equals(LSString *s1, LSString *s2) {
    return strcmp(s1->data, s2->data) == 0;
}

LSString* ls_string_to_upper(LSString *s) {
    LSString *result = ls_malloc(sizeof(LSString));
    result->length = s->length;
    result->capacity = s->capacity;
    result->data = ls_malloc(result->capacity);
    
    for (size_t i = 0; i < s->length; i++) {
        result->data[i] = toupper(s->data[i]);
    }
    result->data[s->length] = '\0';
    result->ref_count = 1;
    return result;
}

LSString* ls_string_to_lower(LSString *s) {
    LSString *result = ls_malloc(sizeof(LSString));
    result->length = s->length;
    result->capacity = s->capacity;
    result->data = ls_malloc(result->capacity);
    
    for (size_t i = 0; i < s->length; i++) {
        result->data[i] = tolower(s->data[i]);
    }
    result->data[s->length] = '\0';
    result->ref_count = 1;
    return result;
}

LSString* ls_string_trim(LSString *s) {
    size_t start = 0;
    size_t end = s->length;
    
    while (start < end && isspace(s->data[start])) start++;
    while (end > start && isspace(s->data[end - 1])) end--;
    
    return ls_string_substring(s, start, end - start);
}

void ls_string_free(LSString *s) {
    if (!s) return;
    s->ref_count--;
    if (s->ref_count <= 0) {
        ls_free(s->data);
        ls_free(s);
    }
}

/* ============================================================================
 * ARRAY OPERATIONS
 * ============================================================================
 */

typedef struct {
    void **data;
    size_t length;
    size_t capacity;
    int ref_count;
} LSArray;

LSArray* ls_array_new(size_t capacity) {
    LSArray *arr = ls_malloc(sizeof(LSArray));
    arr->capacity = capacity > 0 ? capacity : 16;
    arr->length = 0;
    arr->data = ls_malloc(sizeof(void*) * arr->capacity);
    arr->ref_count = 1;
    return arr;
}

void ls_array_push(LSArray *arr, void *element) {
    if (arr->length >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = ls_realloc(arr->data, sizeof(void*) * arr->capacity);
    }
    arr->data[arr->length++] = element;
}

void* ls_array_pop(LSArray *arr) {
    if (arr->length == 0) return NULL;
    return arr->data[--arr->length];
}

void* ls_array_get(LSArray *arr, size_t index) {
    if (index >= arr->length) {
        fprintf(stderr, "Error: Array index out of bounds\n");
        return NULL;
    }
    return arr->data[index];
}

void ls_array_set(LSArray *arr, size_t index, void *element) {
    if (index >= arr->length) {
        fprintf(stderr, "Error: Array index out of bounds\n");
        return;
    }
    arr->data[index] = element;
}

LSArray* ls_array_slice(LSArray *arr, size_t start, size_t end) {
    if (start >= arr->length) return ls_array_new(0);
    if (end > arr->length) end = arr->length;
    
    size_t new_len = end - start;
    LSArray *result = ls_array_new(new_len);
    result->length = new_len;
    
    for (size_t i = 0; i < new_len; i++) {
        result->data[i] = arr->data[start + i];
    }
    
    return result;
}

LSArray* ls_array_concat(LSArray *arr1, LSArray *arr2) {
    size_t new_len = arr1->length + arr2->length;
    LSArray *result = ls_array_new(new_len);
    result->length = new_len;
    
    for (size_t i = 0; i < arr1->length; i++) {
        result->data[i] = arr1->data[i];
    }
    for (size_t i = 0; i < arr2->length; i++) {
        result->data[arr1->length + i] = arr2->data[i];
    }
    
    return result;
}

void ls_array_free(LSArray *arr) {
    if (!arr) return;
    arr->ref_count--;
    if (arr->ref_count <= 0) {
        ls_free(arr->data);
        ls_free(arr);
    }
}

/* ============================================================================
 * HASH MAP OPERATIONS
 * ============================================================================
 */

typedef struct HashMapEntry {
    char *key;
    void *value;
    struct HashMapEntry *next;
} HashMapEntry;

typedef struct {
    HashMapEntry **buckets;
    size_t bucket_count;
    size_t size;
    int ref_count;
} LSHashMap;

static size_t hash_string(const char *str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

LSHashMap* ls_hashmap_new(size_t bucket_count) {
    LSHashMap *map = ls_malloc(sizeof(LSHashMap));
    map->bucket_count = bucket_count > 0 ? bucket_count : 16;
    map->buckets = ls_malloc(sizeof(HashMapEntry*) * map->bucket_count);
    for (size_t i = 0; i < map->bucket_count; i++) {
        map->buckets[i] = NULL;
    }
    map->size = 0;
    map->ref_count = 1;
    return map;
}

void ls_hashmap_set(LSHashMap *map, const char *key, void *value) {
    size_t hash = hash_string(key);
    size_t index = hash % map->bucket_count;
    
    HashMapEntry *entry = map->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }
    
    HashMapEntry *new_entry = ls_malloc(sizeof(HashMapEntry));
    new_entry->key = ls_malloc(strlen(key) + 1);
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next = map->buckets[index];
    map->buckets[index] = new_entry;
    map->size++;
}

void* ls_hashmap_get(LSHashMap *map, const char *key) {
    size_t hash = hash_string(key);
    size_t index = hash % map->bucket_count;
    
    HashMapEntry *entry = map->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;
}

bool ls_hashmap_has(LSHashMap *map, const char *key) {
    return ls_hashmap_get(map, key) != NULL;
}

void ls_hashmap_delete(LSHashMap *map, const char *key) {
    size_t hash = hash_string(key);
    size_t index = hash % map->bucket_count;
    
    HashMapEntry **entry_ptr = &map->buckets[index];
    while (*entry_ptr) {
        if (strcmp((*entry_ptr)->key, key) == 0) {
            HashMapEntry *to_delete = *entry_ptr;
            *entry_ptr = to_delete->next;
            ls_free(to_delete->key);
            ls_free(to_delete);
            map->size--;
            return;
        }
        entry_ptr = &(*entry_ptr)->next;
    }
}

void ls_hashmap_free(LSHashMap *map) {
    if (!map) return;
    map->ref_count--;
    if (map->ref_count <= 0) {
        for (size_t i = 0; i < map->bucket_count; i++) {
            HashMapEntry *entry = map->buckets[i];
            while (entry) {
                HashMapEntry *next = entry->next;
                ls_free(entry->key);
                ls_free(entry);
                entry = next;
            }
        }
        ls_free(map->buckets);
        ls_free(map);
    }
}

/* ============================================================================
 * FILE I/O OPERATIONS
 * ============================================================================
 */

typedef struct {
    FILE *handle;
    char *path;
    char *mode;
    int ref_count;
} LSFile;

LSFile* ls_file_open(const char *path, const char *mode) {
    FILE *handle = fopen(path, mode);
    if (!handle) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", path);
        return NULL;
    }
    
    LSFile *file = ls_malloc(sizeof(LSFile));
    file->handle = handle;
    file->path = ls_malloc(strlen(path) + 1);
    strcpy(file->path, path);
    file->mode = ls_malloc(strlen(mode) + 1);
    strcpy(file->mode, mode);
    file->ref_count = 1;
    return file;
}

LSString* ls_file_read(LSFile *file) {
    if (!file || !file->handle) return NULL;
    
    fseek(file->handle, 0, SEEK_END);
    long size = ftell(file->handle);
    fseek(file->handle, 0, SEEK_SET);
    
    char *buffer = ls_malloc(size + 1);
    fread(buffer, 1, size, file->handle);
    buffer[size] = '\0';
    
    LSString *result = ls_string_new(buffer);
    ls_free(buffer);
    return result;
}

LSString* ls_file_read_line(LSFile *file) {
    if (!file || !file->handle) return NULL;
    
    char buffer[4096];
    if (fgets(buffer, sizeof(buffer), file->handle)) {
        return ls_string_new(buffer);
    }
    return NULL;
}

void ls_file_write(LSFile *file, LSString *content) {
    if (!file || !file->handle || !content) return;
    fwrite(content->data, 1, content->length, file->handle);
}

void ls_file_close(LSFile *file) {
    if (!file) return;
    file->ref_count--;
    if (file->ref_count <= 0) {
        if (file->handle) fclose(file->handle);
        ls_free(file->path);
        ls_free(file->mode);
        ls_free(file);
    }
}

/* ============================================================================
 * MATH OPERATIONS
 * ============================================================================
 */

double ls_math_abs(double x) {
    return fabs(x);
}

double ls_math_sqrt(double x) {
    return sqrt(x);
}

double ls_math_pow(double base, double exp) {
    return pow(base, exp);
}

double ls_math_sin(double x) {
    return sin(x);
}

double ls_math_cos(double x) {
    return cos(x);
}

double ls_math_tan(double x) {
    return tan(x);
}

double ls_math_floor(double x) {
    return floor(x);
}

double ls_math_ceil(double x) {
    return ceil(x);
}

double ls_math_round(double x) {
    return round(x);
}

double ls_math_log(double x) {
    return log(x);
}

double ls_math_log10(double x) {
    return log10(x);
}

double ls_math_exp(double x) {
    return exp(x);
}

int64_t ls_math_min_int(int64_t a, int64_t b) {
    return a < b ? a : b;
}

int64_t ls_math_max_int(int64_t a, int64_t b) {
    return a > b ? a : b;
}

double ls_math_min_float(double a, double b) {
    return a < b ? a : b;
}

double ls_math_max_float(double a, double b) {
    return a > b ? a : b;
}

/* ============================================================================
 * RANDOM NUMBER GENERATION
 * ============================================================================
 */

static bool random_initialized = false;

void ls_random_init() {
    if (!random_initialized) {
        srand(time(NULL));
        random_initialized = true;
    }
}

int64_t ls_random_int(int64_t min, int64_t max) {
    ls_random_init();
    return min + (rand() % (max - min + 1));
}

double ls_random_float() {
    ls_random_init();
    return (double)rand() / RAND_MAX;
}

double ls_random_range(double min, double max) {
    return min + ls_random_float() * (max - min);
}

/* ============================================================================
 * TIME OPERATIONS
 * ============================================================================
 */

int64_t ls_time_now() {
    return (int64_t)time(NULL);
}

int64_t ls_time_milliseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void ls_time_sleep(int64_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/* ============================================================================
 * CONSOLE I/O
 * ============================================================================
 */

void ls_print(LSString *str) {
    if (str && str->data) {
        printf("%s", str->data);
    }
}

void ls_println(LSString *str) {
    if (str && str->data) {
        printf("%s\n", str->data);
    } else {
        printf("\n");
    }
}

void ls_print_int(int64_t value) {
    printf("%lld", value);
}

void ls_print_float(double value) {
    printf("%f", value);
}

void ls_print_bool(bool value) {
    printf("%s", value ? "true" : "false");
}

LSString* ls_input() {
    char buffer[4096];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return ls_string_new(buffer);
    }
    return ls_string_new("");
}

/* ============================================================================
 * TYPE CONVERSION
 * ============================================================================
 */

int64_t ls_string_to_int(LSString *str) {
    if (!str || !str->data) return 0;
    return atoll(str->data);
}

double ls_string_to_float(LSString *str) {
    if (!str || !str->data) return 0.0;
    return atof(str->data);
}

bool ls_string_to_bool(LSString *str) {
    if (!str || !str->data) return false;
    return strcmp(str->data, "true") == 0 || strcmp(str->data, "1") == 0;
}

LSString* ls_int_to_string(int64_t value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld", value);
    return ls_string_new(buffer);
}

LSString* ls_float_to_string(double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%f", value);
    return ls_string_new(buffer);
}

LSString* ls_bool_to_string(bool value) {
    return ls_string_new(value ? "true" : "false");
}

/* ============================================================================
 * ERROR HANDLING
 * ============================================================================
 */

typedef struct {
    char *message;
    char *file;
    int line;
    int ref_count;
} LSError;

LSError* ls_error_new(const char *message, const char *file, int line) {
    LSError *err = ls_malloc(sizeof(LSError));
    err->message = ls_malloc(strlen(message) + 1);
    strcpy(err->message, message);
    err->file = ls_malloc(strlen(file) + 1);
    strcpy(err->file, file);
    err->line = line;
    err->ref_count = 1;
    return err;
}

void ls_error_print(LSError *err) {
    if (!err) return;
    fprintf(stderr, "Error at %s:%d: %s\n", err->file, err->line, err->message);
}

void ls_error_free(LSError *err) {
    if (!err) return;
    err->ref_count--;
    if (err->ref_count <= 0) {
        ls_free(err->message);
        ls_free(err->file);
        ls_free(err);
    }
}

void ls_panic(const char *message) {
    fprintf(stderr, "Panic: %s\n", message);
    exit(1);
}

/* ============================================================================
 * ASSERTION
 * ============================================================================
 */

void ls_assert(bool condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "Assertion failed: %s\n", message);
        exit(1);
    }
}

/* ============================================================================
 * RUNTIME INITIALIZATION
 * ============================================================================
 */

void ls_runtime_init() {
    ls_random_init();
}

void ls_runtime_cleanup() {
    /* Free all remaining memory blocks */
    while (memory_blocks) {
        MemoryBlock *next = memory_blocks->next;
        free(memory_blocks->ptr);
        free(memory_blocks);
        memory_blocks = next;
    }
}

/* ============================================================================
 * GARBAGE COLLECTION (Simple Mark-and-Sweep)
 * ============================================================================
 */

typedef struct GCObject {
    void *ptr;
    bool marked;
    struct GCObject *next;
} GCObject;

static GCObject *gc_objects = NULL;
static size_t gc_threshold = 1024 * 1024; /* 1 MB */
static size_t gc_allocated = 0;

void* ls_gc_alloc(size_t size) {
    if (gc_allocated > gc_threshold) {
        ls_gc_collect();
    }
    
    void *ptr = ls_malloc(size);
    
    GCObject *obj = malloc(sizeof(GCObject));
    obj->ptr = ptr;
    obj->marked = false;
    obj->next = gc_objects;
    gc_objects = obj;
    
    gc_allocated += size;
    
    return ptr;
}

void ls_gc_mark(void *ptr) {
    GCObject *obj = gc_objects;
    while (obj) {
        if (obj->ptr == ptr) {
            obj->marked = true;
            return;
        }
        obj = obj->next;
    }
}

void ls_gc_collect() {
    /* Sweep phase */
    GCObject **obj_ptr = &gc_objects;
    while (*obj_ptr) {
        GCObject *obj = *obj_ptr;
        if (!obj->marked) {
            *obj_ptr = obj->next;
            ls_free(obj->ptr);
            free(obj);
        } else {
            obj->marked = false;
            obj_ptr = &obj->next;
        }
    }
    
    gc_allocated = 0;
}

/* ============================================================================
 * ASYNC RUNTIME (Simple Event Loop)
 * ============================================================================
 */

typedef struct Task {
    void (*func)(void*);
    void *arg;
    struct Task *next;
} Task;

static Task *task_queue = NULL;

void ls_async_schedule(void (*func)(void*), void *arg) {
    Task *task = malloc(sizeof(Task));
    task->func = func;
    task->arg = arg;
    task->next = task_queue;
    task_queue = task;
}

void ls_async_run() {
    while (task_queue) {
        Task *task = task_queue;
        task_queue = task->next;
        task->func(task->arg);
        free(task);
    }
}

/* ============================================================================
 * REFLECTION SUPPORT
 * ============================================================================
 */

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_ARRAY,
    TYPE_OBJECT
} LSType;

typedef struct {
    LSType type;
    char *name;
    size_t size;
} LSTypeInfo;

LSTypeInfo* ls_typeof(void *ptr) {
    /* This would need more sophisticated implementation */
    LSTypeInfo *info = ls_malloc(sizeof(LSTypeInfo));
    info->type = TYPE_OBJECT;
    info->name = ls_malloc(8);
    strcpy(info->name, "unknown");
    info->size = 0;
    return info;
}

/* ============================================================================
 * STANDARD LIBRARY FUNCTIONS
 * ============================================================================
 */

void ls_exit(int code) {
    ls_runtime_cleanup();
    exit(code);
}

LSString* ls_get_env(const char *name) {
    char *value = getenv(name);
    if (value) {
        return ls_string_new(value);
    }
    return NULL;
}

void ls_set_env(const char *name, const char *value) {
    setenv(name, value, 1);
}

/* ============================================================================
 * DEBUGGING UTILITIES
 * ============================================================================
 */

void ls_debug_print(const char *message) {
    fprintf(stderr, "[DEBUG] %s\n", message);
}

void ls_debug_dump_memory() {
    printf("\n=== Memory Dump ===\n");
    MemoryBlock *block = memory_blocks;
    int count = 0;
    while (block) {
        printf("Block %d: %p (%zu bytes, refcount=%d)\n", 
               count++, block->ptr, block->size, block->ref_count);
        block = block->next;
    }
    printf("==================\n");
}


/* ============================================================================
 * EXTENDED RUNTIME - ADVANCED COLLECTIONS
 * ============================================================================
 */

/* Binary Search Tree */
typedef struct BSTNode {
    int key;
    void *value;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct {
    BSTNode *root;
    int size;
} BSTree;

BSTree* ls_bst_new() {
    BSTree *tree = ls_malloc(sizeof(BSTree));
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

BSTNode* ls_bst_create_node(int key, void *value) {
    BSTNode *node = ls_malloc(sizeof(BSTNode));
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

BSTNode* ls_bst_insert_helper(BSTNode *node, int key, void *value) {
    if (!node) {
        return ls_bst_create_node(key, value);
    }
    
    if (key < node->key) {
        node->left = ls_bst_insert_helper(node->left, key, value);
    } else if (key > node->key) {
        node->right = ls_bst_insert_helper(node->right, key, value);
    } else {
        node->value = value;
    }
    
    return node;
}

void ls_bst_insert(BSTree *tree, int key, void *value) {
    tree->root = ls_bst_insert_helper(tree->root, key, value);
    tree->size++;
}

void* ls_bst_search(BSTree *tree, int key) {
    BSTNode *current = tree->root;
    
    while (current) {
        if (key == current->key) {
            return current->value;
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return NULL;
}

/* Red-Black Tree */
typedef enum { RED, BLACK } RBColor;

typedef struct RBNode {
    int key;
    void *value;
    RBColor color;
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
} RBNode;

typedef struct {
    RBNode *root;
    RBNode *nil;
    int size;
} RBTree;

RBTree* ls_rbtree_new() {
    RBTree *tree = ls_malloc(sizeof(RBTree));
    tree->nil = ls_malloc(sizeof(RBNode));
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil->right = tree->nil->parent = NULL;
    tree->root = tree->nil;
    tree->size = 0;
    return tree;
}

void ls_rbtree_rotate_left(RBTree *tree, RBNode *x) {
    RBNode *y = x->right;
    x->right = y->left;
    
    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    
    y->parent = x->parent;
    
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    
    y->left = x;
    x->parent = y;
}

void ls_rbtree_rotate_right(RBTree *tree, RBNode *y) {
    RBNode *x = y->left;
    y->left = x->right;
    
    if (x->right != tree->nil) {
        x->right->parent = y;
    }
    
    x->parent = y->parent;
    
    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    
    x->right = y;
    y->parent = x;
}

/* AVL Tree */
typedef struct AVLNode {
    int key;
    void *value;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

typedef struct {
    AVLNode *root;
    int size;
} AVLTree;

int ls_avl_height(AVLNode *node) {
    return node ? node->height : 0;
}

int ls_avl_balance_factor(AVLNode *node) {
    return node ? ls_avl_height(node->left) - ls_avl_height(node->right) : 0;
}

void ls_avl_update_height(AVLNode *node) {
    if (node) {
        int left_height = ls_avl_height(node->left);
        int right_height = ls_avl_height(node->right);
        node->height = 1 + (left_height > right_height ? left_height : right_height);
    }
}

AVLNode* ls_avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    ls_avl_update_height(y);
    ls_avl_update_height(x);
    
    return x;
}

AVLNode* ls_avl_rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    ls_avl_update_height(x);
    ls_avl_update_height(y);
    
    return y;
}

/* Trie (Prefix Tree) */
typedef struct TrieNode {
    struct TrieNode *children[26];
    bool is_end_of_word;
    void *value;
} TrieNode;

typedef struct {
    TrieNode *root;
    int size;
} Trie;

Trie* ls_trie_new() {
    Trie *trie = ls_malloc(sizeof(Trie));
    trie->root = ls_malloc(sizeof(TrieNode));
    for (int i = 0; i < 26; i++) {
        trie->root->children[i] = NULL;
    }
    trie->root->is_end_of_word = false;
    trie->root->value = NULL;
    trie->size = 0;
    return trie;
}

void ls_trie_insert(Trie *trie, const char *word, void *value) {
    TrieNode *current = trie->root;
    
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (index < 0 || index >= 26) continue;
        
        if (!current->children[index]) {
            current->children[index] = ls_malloc(sizeof(TrieNode));
            for (int j = 0; j < 26; j++) {
                current->children[index]->children[j] = NULL;
            }
            current->children[index]->is_end_of_word = false;
            current->children[index]->value = NULL;
        }
        
        current = current->children[index];
    }
    
    current->is_end_of_word = true;
    current->value = value;
    trie->size++;
}

void* ls_trie_search(Trie *trie, const char *word) {
    TrieNode *current = trie->root;
    
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (index < 0 || index >= 26 || !current->children[index]) {
            return NULL;
        }
        current = current->children[index];
    }
    
    return current->is_end_of_word ? current->value : NULL;
}

/* Priority Queue (Min Heap) */
typedef struct {
    int *keys;
    void **values;
    int size;
    int capacity;
} PriorityQueue;

PriorityQueue* ls_pq_new(int capacity) {
    PriorityQueue *pq = ls_malloc(sizeof(PriorityQueue));
    pq->keys = ls_malloc(sizeof(int) * capacity);
    pq->values = ls_malloc(sizeof(void*) * capacity);
    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

void ls_pq_swap(PriorityQueue *pq, int i, int j) {
    int temp_key = pq->keys[i];
    pq->keys[i] = pq->keys[j];
    pq->keys[j] = temp_key;
    
    void *temp_val = pq->values[i];
    pq->values[i] = pq->values[j];
    pq->values[j] = temp_val;
}

void ls_pq_heapify_up(PriorityQueue *pq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (pq->keys[index] >= pq->keys[parent]) break;
        ls_pq_swap(pq, index, parent);
        index = parent;
    }
}

void ls_pq_heapify_down(PriorityQueue *pq, int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        
        if (left < pq->size && pq->keys[left] < pq->keys[smallest]) {
            smallest = left;
        }
        if (right < pq->size && pq->keys[right] < pq->keys[smallest]) {
            smallest = right;
        }
        
        if (smallest == index) break;
        
        ls_pq_swap(pq, index, smallest);
        index = smallest;
    }
}

void ls_pq_insert(PriorityQueue *pq, int key, void *value) {
    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        pq->keys = ls_realloc(pq->keys, sizeof(int) * pq->capacity);
        pq->values = ls_realloc(pq->values, sizeof(void*) * pq->capacity);
    }
    
    pq->keys[pq->size] = key;
    pq->values[pq->size] = value;
    ls_pq_heapify_up(pq, pq->size);
    pq->size++;
}

void* ls_pq_extract_min(PriorityQueue *pq) {
    if (pq->size == 0) return NULL;
    
    void *min_value = pq->values[0];
    pq->size--;
    
    if (pq->size > 0) {
        pq->keys[0] = pq->keys[pq->size];
        pq->values[0] = pq->values[pq->size];
        ls_pq_heapify_down(pq, 0);
    }
    
    return min_value;
}

/* Graph Data Structure */
typedef struct GraphNode {
    int vertex;
    int weight;
    struct GraphNode *next;
} GraphNode;

typedef struct {
    GraphNode **adjacency_list;
    int vertex_count;
} Graph;

Graph* ls_graph_new(int vertex_count) {
    Graph *graph = ls_malloc(sizeof(Graph));
    graph->vertex_count = vertex_count;
    graph->adjacency_list = ls_malloc(sizeof(GraphNode*) * vertex_count);
    
    for (int i = 0; i < vertex_count; i++) {
        graph->adjacency_list[i] = NULL;
    }
    
    return graph;
}

void ls_graph_add_edge(Graph *graph, int src, int dest, int weight) {
    GraphNode *node = ls_malloc(sizeof(GraphNode));
    node->vertex = dest;
    node->weight = weight;
    node->next = graph->adjacency_list[src];
    graph->adjacency_list[src] = node;
}

/* Bloom Filter */
typedef struct {
    uint8_t *bits;
    int size;
    int hash_count;
} BloomFilter;

BloomFilter* ls_bloom_new(int size, int hash_count) {
    BloomFilter *filter = ls_malloc(sizeof(BloomFilter));
    filter->size = size;
    filter->hash_count = hash_count;
    filter->bits = ls_malloc((size + 7) / 8);
    memset(filter->bits, 0, (size + 7) / 8);
    return filter;
}

uint32_t ls_bloom_hash(const char *str, int seed) {
    uint32_t hash = seed;
    while (*str) {
        hash = hash * 31 + *str++;
    }
    return hash;
}

void ls_bloom_add(BloomFilter *filter, const char *item) {
    for (int i = 0; i < filter->hash_count; i++) {
        uint32_t hash = ls_bloom_hash(item, i) % filter->size;
        filter->bits[hash / 8] |= (1 << (hash % 8));
    }
}

bool ls_bloom_contains(BloomFilter *filter, const char *item) {
    for (int i = 0; i < filter->hash_count; i++) {
        uint32_t hash = ls_bloom_hash(item, i) % filter->size;
        if (!(filter->bits[hash / 8] & (1 << (hash % 8)))) {
            return false;
        }
    }
    return true;
}

/* Skip List */
#define MAX_SKIP_LEVEL 16

typedef struct SkipNode {
    int key;
    void *value;
    struct SkipNode **forward;
} SkipNode;

typedef struct {
    SkipNode *header;
    int level;
    int size;
} SkipList;

SkipList* ls_skiplist_new() {
    SkipList *list = ls_malloc(sizeof(SkipList));
    list->header = ls_malloc(sizeof(SkipNode));
    list->header->forward = ls_malloc(sizeof(SkipNode*) * MAX_SKIP_LEVEL);
    for (int i = 0; i < MAX_SKIP_LEVEL; i++) {
        list->header->forward[i] = NULL;
    }
    list->level = 0;
    list->size = 0;
    return list;
}

int ls_skiplist_random_level() {
    int level = 0;
    while (rand() % 2 && level < MAX_SKIP_LEVEL - 1) {
        level++;
    }
    return level;
}

void ls_skiplist_insert(SkipList *list, int key, void *value) {
    SkipNode *update[MAX_SKIP_LEVEL];
    SkipNode *current = list->header;
    
    for (int i = list->level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    int new_level = ls_skiplist_random_level();
    if (new_level > list->level) {
        for (int i = list->level + 1; i <= new_level; i++) {
            update[i] = list->header;
        }
        list->level = new_level;
    }
    
    SkipNode *node = ls_malloc(sizeof(SkipNode));
    node->key = key;
    node->value = value;
    node->forward = ls_malloc(sizeof(SkipNode*) * (new_level + 1));
    
    for (int i = 0; i <= new_level; i++) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
    
    list->size++;
}

void* ls_skiplist_search(SkipList *list, int key) {
    SkipNode *current = list->header;
    
    for (int i = list->level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }
    
    current = current->forward[0];
    
    if (current && current->key == key) {
        return current->value;
    }
    
    return NULL;
}

/* LRU Cache */
typedef struct LRUNode {
    int key;
    void *value;
    struct LRUNode *prev;
    struct LRUNode *next;
} LRUNode;

typedef struct {
    LRUNode *head;
    LRUNode *tail;
    LSHashMap *map;
    int capacity;
    int size;
} LRUCache;

LRUCache* ls_lru_new(int capacity) {
    LRUCache *cache = ls_malloc(sizeof(LRUCache));
    cache->head = ls_malloc(sizeof(LRUNode));
    cache->tail = ls_malloc(sizeof(LRUNode));
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    cache->map = ls_hashmap_new(capacity);
    cache->capacity = capacity;
    cache->size = 0;
    return cache;
}

void ls_lru_remove_node(LRUNode *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void ls_lru_add_to_head(LRUCache *cache, LRUNode *node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

void* ls_lru_get(LRUCache *cache, int key) {
    char key_str[32];
    snprintf(key_str, sizeof(key_str), "%d", key);
    
    LRUNode *node = (LRUNode*)ls_hashmap_get(cache->map, key_str);
    if (!node) return NULL;
    
    ls_lru_remove_node(node);
    ls_lru_add_to_head(cache, node);
    
    return node->value;
}

void ls_lru_put(LRUCache *cache, int key, void *value) {
    char key_str[32];
    snprintf(key_str, sizeof(key_str), "%d", key);
    
    LRUNode *node = (LRUNode*)ls_hashmap_get(cache->map, key_str);
    
    if (node) {
        node->value = value;
        ls_lru_remove_node(node);
        ls_lru_add_to_head(cache, node);
    } else {
        if (cache->size >= cache->capacity) {
            LRUNode *tail_node = cache->tail->prev;
            ls_lru_remove_node(tail_node);
            
            char tail_key_str[32];
            snprintf(tail_key_str, sizeof(tail_key_str), "%d", tail_node->key);
            ls_hashmap_delete(cache->map, tail_key_str);
            
            ls_free(tail_node);
            cache->size--;
        }
        
        LRUNode *new_node = ls_malloc(sizeof(LRUNode));
        new_node->key = key;
        new_node->value = value;
        
        ls_lru_add_to_head(cache, new_node);
        ls_hashmap_set(cache->map, key_str, new_node);
        cache->size++;
    }
}

/* Disjoint Set (Union-Find) */
typedef struct {
    int *parent;
    int *rank;
    int size;
} DisjointSet;

DisjointSet* ls_disjoint_set_new(int size) {
    DisjointSet *ds = ls_malloc(sizeof(DisjointSet));
    ds->parent = ls_malloc(sizeof(int) * size);
    ds->rank = ls_malloc(sizeof(int) * size);
    ds->size = size;
    
    for (int i = 0; i < size; i++) {
        ds->parent[i] = i;
        ds->rank[i] = 0;
    }
    
    return ds;
}

int ls_disjoint_set_find(DisjointSet *ds, int x) {
    if (ds->parent[x] != x) {
        ds->parent[x] = ls_disjoint_set_find(ds, ds->parent[x]);
    }
    return ds->parent[x];
}

void ls_disjoint_set_union(DisjointSet *ds, int x, int y) {
    int root_x = ls_disjoint_set_find(ds, x);
    int root_y = ls_disjoint_set_find(ds, y);
    
    if (root_x == root_y) return;
    
    if (ds->rank[root_x] < ds->rank[root_y]) {
        ds->parent[root_x] = root_y;
    } else if (ds->rank[root_x] > ds->rank[root_y]) {
        ds->parent[root_y] = root_x;
    } else {
        ds->parent[root_y] = root_x;
        ds->rank[root_x]++;
    }
}

/* Segment Tree */
typedef struct {
    int *tree;
    int size;
} SegmentTree;

SegmentTree* ls_segtree_new(int *arr, int n) {
    SegmentTree *st = ls_malloc(sizeof(SegmentTree));
    st->size = n;
    st->tree = ls_malloc(sizeof(int) * (4 * n));
    return st;
}

int ls_segtree_build(SegmentTree *st, int *arr, int node, int start, int end) {
    if (start == end) {
        st->tree[node] = arr[start];
        return st->tree[node];
    }
    
    int mid = (start + end) / 2;
    int left_sum = ls_segtree_build(st, arr, 2 * node, start, mid);
    int right_sum = ls_segtree_build(st, arr, 2 * node + 1, mid + 1, end);
    
    st->tree[node] = left_sum + right_sum;
    return st->tree[node];
}

int ls_segtree_query(SegmentTree *st, int node, int start, int end, int l, int r) {
    if (r < start || end < l) return 0;
    if (l <= start && end <= r) return st->tree[node];
    
    int mid = (start + end) / 2;
    int left_sum = ls_segtree_query(st, 2 * node, start, mid, l, r);
    int right_sum = ls_segtree_query(st, 2 * node + 1, mid + 1, end, l, r);
    
    return left_sum + right_sum;
}

void ls_segtree_update(SegmentTree *st, int node, int start, int end, int idx, int val) {
    if (start == end) {
        st->tree[node] = val;
        return;
    }
    
    int mid = (start + end) / 2;
    if (idx <= mid) {
        ls_segtree_update(st, 2 * node, start, mid, idx, val);
    } else {
        ls_segtree_update(st, 2 * node + 1, mid + 1, end, idx, val);
    }
    
    st->tree[node] = st->tree[2 * node] + st->tree[2 * node + 1];
}

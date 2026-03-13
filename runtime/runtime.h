/* ============================================================================
 * L# Programming Language - Runtime Library Header
 * ============================================================================
 * File: runtime.h
 * Purpose: Runtime function declarations and type definitions
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#ifndef LSHARP_RUNTIME_H
#define LSHARP_RUNTIME_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Forward declarations */
typedef struct LSString LSString;
typedef struct LSArray LSArray;
typedef struct LSHashMap LSHashMap;
typedef struct LSFile LSFile;
typedef struct LSError LSError;

/* Memory Management */
void* ls_malloc(size_t size);
void* ls_realloc(void *ptr, size_t new_size);
void ls_free(void *ptr);
void ls_incref(void *ptr);
void ls_decref(void *ptr);
void ls_print_memory_stats(void);

/* String Operations */
LSString* ls_string_new(const char *str);
LSString* ls_string_concat(LSString *s1, LSString *s2);
LSString* ls_string_substring(LSString *s, size_t start, size_t length);
int ls_string_compare(LSString *s1, LSString *s2);
bool ls_string_equals(LSString *s1, LSString *s2);
LSString* ls_string_to_upper(LSString *s);
LSString* ls_string_to_lower(LSString *s);
LSString* ls_string_trim(LSString *s);
void ls_string_free(LSString *s);

/* Array Operations */
LSArray* ls_array_new(size_t capacity);
void ls_array_push(LSArray *arr, void *element);
void* ls_array_pop(LSArray *arr);
void* ls_array_get(LSArray *arr, size_t index);
void ls_array_set(LSArray *arr, size_t index, void *element);
LSArray* ls_array_slice(LSArray *arr, size_t start, size_t end);
LSArray* ls_array_concat(LSArray *arr1, LSArray *arr2);
void ls_array_free(LSArray *arr);

/* Hash Map Operations */
LSHashMap* ls_hashmap_new(size_t bucket_count);
void ls_hashmap_set(LSHashMap *map, const char *key, void *value);
void* ls_hashmap_get(LSHashMap *map, const char *key);
bool ls_hashmap_has(LSHashMap *map, const char *key);
void ls_hashmap_delete(LSHashMap *map, const char *key);
void ls_hashmap_free(LSHashMap *map);

/* File I/O Operations */
LSFile* ls_file_open(const char *path, const char *mode);
LSString* ls_file_read(LSFile *file);
LSString* ls_file_read_line(LSFile *file);
void ls_file_write(LSFile *file, LSString *content);
void ls_file_close(LSFile *file);

/* Math Operations */
double ls_math_abs(double x);
double ls_math_sqrt(double x);
double ls_math_pow(double base, double exp);
double ls_math_sin(double x);
double ls_math_cos(double x);
double ls_math_tan(double x);
double ls_math_floor(double x);
double ls_math_ceil(double x);
double ls_math_round(double x);
double ls_math_log(double x);
double ls_math_log10(double x);
double ls_math_exp(double x);
int64_t ls_math_min_int(int64_t a, int64_t b);
int64_t ls_math_max_int(int64_t a, int64_t b);
double ls_math_min_float(double a, double b);
double ls_math_max_float(double a, double b);

/* Random Number Generation */
void ls_random_init(void);
int64_t ls_random_int(int64_t min, int64_t max);
double ls_random_float(void);
double ls_random_range(double min, double max);

/* Time Operations */
int64_t ls_time_now(void);
int64_t ls_time_milliseconds(void);
void ls_time_sleep(int64_t milliseconds);

/* Console I/O */
void ls_print(LSString *str);
void ls_println(LSString *str);
void ls_print_int(int64_t value);
void ls_print_float(double value);
void ls_print_bool(bool value);
LSString* ls_input(void);

/* Type Conversion */
int64_t ls_string_to_int(LSString *str);
double ls_string_to_float(LSString *str);
bool ls_string_to_bool(LSString *str);
LSString* ls_int_to_string(int64_t value);
LSString* ls_float_to_string(double value);
LSString* ls_bool_to_string(bool value);

/* Error Handling */
LSError* ls_error_new(const char *message, const char *file, int line);
void ls_error_print(LSError *err);
void ls_error_free(LSError *err);
void ls_panic(const char *message);

/* Assertion */
void ls_assert(bool condition, const char *message);

/* Runtime Initialization */
void ls_runtime_init(void);
void ls_runtime_cleanup(void);

/* Garbage Collection */
void* ls_gc_alloc(size_t size);
void ls_gc_mark(void *ptr);
void ls_gc_collect(void);

/* Async Runtime */
void ls_async_schedule(void (*func)(void*), void *arg);
void ls_async_run(void);

/* Reflection */
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

LSTypeInfo* ls_typeof(void *ptr);

/* Standard Library */
void ls_exit(int code);
LSString* ls_get_env(const char *name);
void ls_set_env(const char *name, const char *value);

/* Debugging */
void ls_debug_print(const char *message);
void ls_debug_dump_memory(void);

#endif /* LSHARP_RUNTIME_H */

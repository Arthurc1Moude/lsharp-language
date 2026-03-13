/* ============================================================================
 * L# Programming Language - Virtual Machine Header
 * ============================================================================
 * File: vm.h
 * Purpose: VM function declarations and type definitions
 * 
 * Copyright (c) 2026 Moude AI Inc.
 * Licensed under MIT License
 * ============================================================================
 */

#ifndef LSHARP_VM_H
#define LSHARP_VM_H

#include <stdint.h>
#include <stddef.h>

/* VM Initialization and Control */
void vm_init(void);
void vm_load_bytecode(uint8_t *code, size_t size);
void vm_run(void);
void vm_cleanup(void);

/* Debugging */
void vm_enable_debug(void);
void vm_print_stack(void);

#endif /* LSHARP_VM_H */

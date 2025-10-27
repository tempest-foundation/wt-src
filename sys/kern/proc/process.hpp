// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <kstdint.hpp>

#include <kern/memory/memory.hpp>

#define MAX_PROCESSES   256
#define USER_STACK_SIZE (1024 * 1024)  // 1MB stack

typedef enum {
	PROC_STATE_UNUSED = 0,
	PROC_STATE_READY,
	PROC_STATE_RUNNING,
	PROC_STATE_BLOCKED,
	PROC_STATE_ZOMBIE
} process_state_t;

typedef struct process {
	uint32_t        pid;    // Process ID
	process_state_t state;  // Current state

	// Memory management
	pml4_t  *page_table;  // Process page table
	uint64_t stack_base;  // User stack base
	uint64_t heap_base;   // User heap base
	uint64_t heap_end;    // Current heap end

	// CPU state (saved on context switch)
	uint64_t rip;                 // Instruction pointer
	uint64_t rsp;                 // Stack pointer
	uint64_t rbp;                 // Base pointer
	uint64_t rax, rbx, rcx, rdx;  // General purpose registers
	uint64_t rsi, rdi;
	uint64_t r8, r9, r10, r11;
	uint64_t r12, r13, r14, r15;
	uint64_t rflags;  // CPU flags

	// File descriptors (simple array for now)
	void *fd_table[16];

	// Parent/child relationships
	struct process *parent;
	struct process *next;  // Next in process list
} process_t;

namespace proc {
	void       init(void);
	process_t *create_process(void (*entry_point)(void), bool is_kernel);
	void       switch_to_process(process_t *proc);
	process_t *get_current_process(void);
	void       exit_process(int exit_code);
	void       yield(void);
}  // namespace proc

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
#include "process.h"

#include <kstring.h>

#include <dbg/logger.h>
#include <kern/scheduler/scheduler.h>

static process_t  process_table[MAX_PROCESSES];
static process_t *current_process = nullptr;
static uint32_t   next_pid        = 1;

namespace proc {
	void init(void) {
		kstring::memset(process_table, 0, sizeof(process_table));
		logger::info("proc", "Process manager initialized", nullptr);
	}

	process_t *allocate_process(void) {
		for( int i = 0; i < MAX_PROCESSES; i++ ) {
			if( process_table[i].state == PROC_STATE_UNUSED ) {
				process_table[i].pid   = next_pid++;
				process_table[i].state = PROC_STATE_READY;
				return &process_table[i];
			}
		}
		return nullptr;
	}

	process_t *create_process(void (*entry_point)(void), bool is_kernel) {
		process_t *proc = allocate_process();
		if( !proc ) {
			logger::error("proc", "Failed to allocate process", nullptr);
			return nullptr;
		}

		// Allocate page table
		proc->page_table = (pml4_t *) memory::malloc(sizeof(pml4_t));
		if( !proc->page_table ) {
			proc->state = PROC_STATE_UNUSED;
			return nullptr;
		}
		kstring::memset(proc->page_table, 0, sizeof(pml4_t));

		// Copy kernel mappings (higher half)
		// This allows the process to make syscalls
		uint64_t cr3;
		__asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
		pml4_t *kernel_pml4 = (pml4_t *) (cr3 & (uint64_t) ~0xFFF);

		for( int i = 256; i < 512; i++ ) {
			proc->page_table->entries[i] = kernel_pml4->entries[i];
		}

		// Allocate user stack
		proc->stack_base = USER_SPACE_BASE + (1024 * 1024 * 1024);  // 1GB offset
		for( uint64_t i = 0; i < USER_STACK_SIZE; i += PAGE_SIZE ) {
			page_frame_t *frame = memory::allocate_page_frame();
			if( !frame ) {
				logger::error(
				    "proc", "Failed to allocate stack page", nullptr);
				proc->state = PROC_STATE_UNUSED;
				return nullptr;
			}

			uint64_t virt = proc->stack_base + i;
			uint64_t phys = memory::get_physical_addr(frame);
			memory::vm::map_page(
			    virt, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
		}

		// Set initial stack pointer (stack grows down)
		proc->rsp = proc->stack_base + USER_STACK_SIZE - 16;
		proc->rbp = proc->rsp;

		// Set entry point
		proc->rip = (uint64_t) entry_point;

		// Set user-space flags: interrupts enabled, ring 3
		if( !is_kernel ) {
			proc->rflags = 0x202 | (3 << 12);  // IF=1, IOPL=3
		} else {
			proc->rflags = 0x202;  // IF=1, IOPL=0
		}

		// Initialize heap
		proc->heap_base = USER_SPACE_BASE;
		proc->heap_end  = proc->heap_base;

		logger::debug::printf("proc",
		                      "info",
		                      "Created process PID=%u at RIP=0x%llx\n",
		                      proc->pid,
		                      proc->rip);

		return proc;
	}

	process_t *get_current_process(void) {
		return current_process;
	}

	void switch_to_process(process_t *proc) {
		if( !proc || proc->state != PROC_STATE_READY ) {
			return;
		}

		// Save current process state if exists
		if( current_process && current_process->state == PROC_STATE_RUNNING ) {
			current_process->state = PROC_STATE_READY;
			// Context saved by interrupt handler
		}

		// Switch to new process
		current_process = proc;
		proc->state     = PROC_STATE_RUNNING;

		// Switch page table
		uint64_t cr3 = (uint64_t) proc->page_table;
		__asm__ volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");

		logger::debug::printf(
		    "proc", "info", "Switched to process PID=%u\n", proc->pid);
	}

	void exit_process(int exit_code) {
		if( !current_process )
			return;

		logger::debug::printf("proc",
		                      "info",
		                      "Process PID=%u exited with code %d\n",
		                      current_process->pid,
		                      exit_code);

		// Mark process as zombie
		current_process->state = PROC_STATE_ZOMBIE;

		// Remove from scheduler run queue
		scheduler::remove_process(current_process);

		// Clear current process pointer
		current_process = nullptr;

		// Schedule next process to run
		scheduler::schedule();

		// Should never reach here as schedule() switches to another process
		// But if it does (no other processes), halt
		__asm__ volatile("cli; hlt");
	}

	void yield(void) {
		// Yield CPU to another process by invoking the scheduler
		// The scheduler will save current state and switch to next process
		scheduler::schedule();
	}
}  // namespace proc
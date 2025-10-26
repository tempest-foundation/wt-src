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
#include "scheduler.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/idt/idt.h>
#endif
#include <dbg/logger.h>

static process_t *run_queue_head  = nullptr;
static process_t *current_running = nullptr;
static uint64_t   time_slice      = 10;  // 10 timer ticks per process
static uint64_t   current_ticks   = 0;

// Interrupt stack frame structure (pushed by CPU)
struct interrupt_frame {
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

namespace scheduler {
	void init(void) {
		run_queue_head  = nullptr;
		current_running = nullptr;
		current_ticks   = 0;
		logger::info("sched", "Scheduler initialized", nullptr);
	}

	void add_process(process_t *proc) {
		if( !proc || proc->state != PROC_STATE_READY )
			return;

		// Add to end of run queue
		if( !run_queue_head ) {
			run_queue_head = proc;
			proc->next     = nullptr;
		} else {
			process_t *tail = run_queue_head;
			while( tail->next ) {
				tail = tail->next;
			}
			tail->next = proc;
			proc->next = nullptr;
		}

		logger::debug::printf(
		    "sched", "info", "Added process PID=%u to run queue\n", proc->pid);
	}

	void remove_process(process_t *proc) {
		if( !proc )
			return;

		if( run_queue_head == proc ) {
			run_queue_head = proc->next;
			return;
		}

		process_t *prev = run_queue_head;
		while( prev && prev->next != proc ) {
			prev = prev->next;
		}

		if( prev )
			prev->next = proc->next;
	}

	process_t *pick_next(void) {
		// Simple round-robin: return head of queue
		return run_queue_head;
	}

	void save_context(process_t *proc, registers_t *regs) {
		if( !proc || !regs )
			return;

		// Save general purpose registers from registers_t
		proc->rax = regs->rax;
		proc->rbx = regs->rbx;
		proc->rcx = regs->rcx;
		proc->rdx = regs->rdx;
		proc->rsi = regs->rsi;
		proc->rdi = regs->rdi;
		proc->rbp = regs->rbp;
		proc->r8  = regs->r8;
		proc->r9  = regs->r9;
		proc->r10 = regs->r10;
		proc->r11 = regs->r11;
		proc->r12 = regs->r12;
		proc->r13 = regs->r13;
		proc->r14 = regs->r14;
		proc->r15 = regs->r15;

		// The interrupt stack frame is located after registers_t on the stack
		// Stack layout (from top to bottom):
		// - registers_t (what regs points to)
		// - interrupt_frame (RIP, CS, RFLAGS, RSP, SS) <- pushed by CPU
		interrupt_frame *iframe =
		    (interrupt_frame *) ((uint8_t *) regs + sizeof(registers_t));

		proc->rip    = iframe->rip;
		proc->rsp    = iframe->rsp;
		proc->rflags = iframe->rflags;
	}

	void restore_context(process_t *proc, registers_t *regs) {
		if( !proc || !regs )
			return;

		// Restore general purpose registers
		regs->rax = proc->rax;
		regs->rbx = proc->rbx;
		regs->rcx = proc->rcx;
		regs->rdx = proc->rdx;
		regs->rsi = proc->rsi;
		regs->rdi = proc->rdi;
		regs->rbp = proc->rbp;
		regs->r8  = proc->r8;
		regs->r9  = proc->r9;
		regs->r10 = proc->r10;
		regs->r11 = proc->r11;
		regs->r12 = proc->r12;
		regs->r13 = proc->r13;
		regs->r14 = proc->r14;
		regs->r15 = proc->r15;

		// Restore interrupt stack frame
		interrupt_frame *iframe =
		    (interrupt_frame *) ((uint8_t *) regs + sizeof(registers_t));

		iframe->rip    = proc->rip;
		iframe->rsp    = proc->rsp;
		iframe->rflags = proc->rflags;
		// CS and SS are typically not modified during context switch
		// They should remain as user code segment and user data segment
	}

	void schedule(void) {
		// Save current process state
		if( current_running && current_running->state == PROC_STATE_RUNNING ) {
			current_running->state = PROC_STATE_READY;

			// Move to end of queue (round-robin)
			remove_process(current_running);
			add_process(current_running);
		}

		// Pick next process
		process_t *next = pick_next();
		if( !next ) {
			// No processes to schedule - idle
			return;
		}

		// Switch to next process
		current_running = next;
		next->state     = PROC_STATE_RUNNING;

		// Reset time slice
		current_ticks = 0;

		// Switch page table
		uint64_t cr3 = (uint64_t) next->page_table;
		__asm__ volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");

		logger::debug::printf(
		    "sched", "info", "Scheduled process PID=%u\n", next->pid);
	}

	void tick(void) {
		current_ticks++;

		// Preempt if time slice expired
		if( current_ticks >= time_slice )
			schedule();
	}

	process_t *get_current_process(void) {
		return current_running;
	}
}  // namespace scheduler
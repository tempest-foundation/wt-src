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
#include "syscall.h"

#include <kstdio.h>

#include <dbg/logger.h>
#include <kern/syscall/calls/sys.h>

// Global syscall table
static syscall_entry_t syscall_table[SYSCALL_MAX_COUNT];
static uint64_t        syscall_count = 0;

// Initialize the syscall subsystem
namespace syscall {
	// Register a new syscall handler
	void bind(uint64_t          syscall_no,
	          syscall_handler_t handler,
	          const char       *name,
	          uint8_t           arg_count) {
		if( syscall_no >= SYSCALL_MAX_COUNT ) {
			logger::printf("syscall",
			               "error",
			               "Syscall number %llu is out of range\n",
			               syscall_no);
			return;
		}

		if( handler == nullptr ) {
			logger::printf("syscall",
			               "error",
			               "Cannot register NULL handler for syscall %llu\n",
			               syscall_no);
			return;
		}

		// Check if syscall is already registered
		if( syscall_table[syscall_no].handler != nullptr ) {
			logger::printf("syscall",
			               "warn",
			               "Overwriting existing syscall %llu (%s)\n",
			               syscall_no,
			               syscall_table[syscall_no].name);
		} else {
			syscall_count++;
		}

		syscall_table[syscall_no].handler   = handler;
		syscall_table[syscall_no].name      = name;
		syscall_table[syscall_no].arg_count = arg_count;

		logger::printf("syscall",
		               "success",
		               "Registered syscall %llu: %s (args: %u)\n",
		               syscall_no,
		               name,
		               arg_count);
	}

	// Unregister a syscall handler
	void unbind(uint64_t syscall_no) {
		if( syscall_no >= SYSCALL_MAX_COUNT ) {
			logger::printf("syscall",
			               "error",
			               "Syscall number %llu is out of range\n",
			               syscall_no);
			return;
		}

		if( syscall_table[syscall_no].handler == nullptr ) {
			logger::printf("syscall",
			               "warn",
			               "Syscall %llu is not registered\n",
			               syscall_no);
			return;
		}

		logger::printf("syscall",
		               "success",
		               "Unregistered syscall %llu: %s\n",
		               syscall_no,
		               syscall_table[syscall_no].name);

		syscall_table[syscall_no].handler   = nullptr;
		syscall_table[syscall_no].name      = nullptr;
		syscall_table[syscall_no].arg_count = 0;
		syscall_count--;
	}

	void init(void) {
		// Clear the syscall table
		for( uint64_t i = 0; i < SYSCALL_MAX_COUNT; i++ ) {
			syscall_table[i].handler   = nullptr;
			syscall_table[i].name      = nullptr;
			syscall_table[i].arg_count = 0;
		}

		// Register core syscalls
		bind(SYS_EXIT, sys_exit, "exit", 1);
		bind(SYS_GETPID, sys_getpid, "getpid", 0);
		bind(SYS_READ, sys_read, "read", 3);
		bind(SYS_WRITE, sys_write, "write", 3);

		logger::printf("syscall",
		               "success",
		               "Initialized with %llu syscalls\n",
		               syscall_count);
	}

	// Get syscall informationregisters_t *regs
	syscall_entry_t *get_info(uint64_t syscall_no) {
		if( syscall_no >= SYSCALL_MAX_COUNT ) {
			return nullptr;
		}

		if( syscall_table[syscall_no].handler == nullptr ) {
			return nullptr;
		}

		return &syscall_table[syscall_no];
	}

	// Check if syscall is valid
	uint8_t is_valid(uint64_t syscall_no) {
		if( syscall_no >= SYSCALL_MAX_COUNT ) {
			return 0;
		}

		return (syscall_table[syscall_no].handler != nullptr) ? 1 : 0;
	}

	// Main syscall dispatcher (called from assembly)
	void handler(registers_t *regs) {
		// Syscall number is in RAX
		uint64_t syscall_no = regs->rax;

		// Arguments are in RDI, RSI, RDX, RCX, R8, R9
		uint64_t arg0 = regs->rdi;
		uint64_t arg1 = regs->rsi;
		uint64_t arg2 = regs->rdx;
		uint64_t arg3 = regs->rcx;
		uint64_t arg4 = regs->r8;
		uint64_t arg5 = regs->r9;

		// Check if syscall is valid
		if( !is_valid(syscall_no) ) {
			logger::printf("syscall",
			               "error",
			               "Invalid syscall number %llu\n",
			               syscall_no);
			regs->rax = SYSCALL_INVALID;
			return;
		}

		// Get syscall entry
		const syscall_entry_t *entry = get_info(syscall_no);
		if( entry == nullptr ) {
			logger::printf("syscall",
			               "error",
			               "Failed to get syscall info for %llu\n",
			               syscall_no);
			regs->rax = SYSCALL_ERROR;
			return;
		}

		// Log syscall for debugging (can be disabled in production)
		logger::printf("syscall",
		               "notice",
		               "Calling syscall %llu: %s\n",
		               syscall_no,
		               entry->name);

		// Call the syscall handler
		uint64_t result =
		    entry->handler(syscall_no, arg0, arg1, arg2, arg3, arg4, arg5);

		// Return result in RAX
		regs->rax = result;

		logger::printf("syscall",
		               "success",
		               "Syscall %llu returned %llu\n",
		               syscall_no,
		               result);
	}
}  // namespace syscall

// C-wrapper to the assembly files
extern "C" void
    syscall_handler(registers_t *regs) {
	syscall::handler(regs);
}

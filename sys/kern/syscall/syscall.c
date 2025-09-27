// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/syscall/syscall.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "syscall.h"

#include <debug/debug.h>
#include <kern/syscall/calls/sys.h>
#include <lib/kstdio/kstdio.h>

// Global syscall table
static syscall_entry_t syscall_table[SYSCALL_MAX_COUNT];
static kuint64_t       syscall_count = 0;

// Initialize the syscall subsystem
void
    syscall_init (void) {
	// Clear the syscall table
	for (kuint64_t i = 0; i < SYSCALL_MAX_COUNT; i++) {
		syscall_table[i].handler   = KNULL;
		syscall_table[i].name      = KNULL;
		syscall_table[i].arg_count = 0;
	}

	// Register core syscalls
	syscalls.bind(SYS_EXIT, sys_exit, "exit", 1);
	syscalls.bind(SYS_GETPID, sys_getpid, "getpid", 0);
	syscalls.bind(SYS_READ, sys_read, "read", 3);
	syscalls.bind(SYS_WRITE, sys_write, "write", 3);

	debug.printf(
	    "syscall", "success", "Initialized with %llu syscalls\n", syscall_count);
}

// Register a new syscall handler
/*
 * FIXME: When put support for formatting like
 * `kprintf()` in debug, I will go change all
 * Of this bullshit `debug.printf` in errors
 * or warnings into a `debug.err()` or
 * `debug.warn()`.
 */
void
    syscall_bind (kuint64_t         syscall_no,
                  syscall_handler_t handler,
                  const char       *name,
                  kuint8_t          arg_count) {
	if (syscall_no >= SYSCALL_MAX_COUNT) {
		debug.printf("syscall",
		             "error",
		             "Syscall number %llu is out of range\n",
		             syscall_no);
		return;
	}

	if (handler == KNULL) {
		debug.printf("syscall",
		             "error",
		             "Cannot register NULL handler for syscall %llu\n",
		             syscall_no);
		return;
	}

	// Check if syscall is already registered
	if (syscall_table[syscall_no].handler != KNULL) {
		debug.printf("syscall",
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

	debug.printf("syscall",
	             "success",
	             "Registered syscall %llu: %s (args: %u)\n",
	             syscall_no,
	             name,
	             arg_count);
}

// Unregister a syscall handler
void
    syscall_unbind (kuint64_t syscall_no) {
	if (syscall_no >= SYSCALL_MAX_COUNT) {
		debug.printf("syscall",
		             "error",
		             "Syscall number %llu is out of range\n",
		             syscall_no);
		return;
	}

	if (syscall_table[syscall_no].handler == KNULL) {
		debug.printf(
		    "syscall", "warn", "Syscall %llu is not registered\n", syscall_no);
		return;
	}

	debug.printf("syscall",
	             "success",
	             "Unregistered syscall %llu: %s\n",
	             syscall_no,
	             syscall_table[syscall_no].name);

	syscall_table[syscall_no].handler   = KNULL;
	syscall_table[syscall_no].name      = KNULL;
	syscall_table[syscall_no].arg_count = 0;
	syscall_count--;
}

// Get syscall informationregisters_t *regs
syscall_entry_t *
    syscall_get_info (kuint64_t syscall_no) {
	if (syscall_no >= SYSCALL_MAX_COUNT) {
		return KNULL;
	}

	if (syscall_table[syscall_no].handler == KNULL) {
		return KNULL;
	}

	return &syscall_table[syscall_no];
}

// Check if syscall is valid
kuint8_t
    syscall_is_valid (kuint64_t syscall_no) {
	if (syscall_no >= SYSCALL_MAX_COUNT) {
		return 0;
	}

	return (syscall_table[syscall_no].handler != KNULL) ? 1 : 0;
}

// Main syscall dispatcher (called from assembly)
void
    syscall_handler (registers_t *regs) {
	// Syscall number is in RAX
	kuint64_t syscall_no = regs->rax;

	// Arguments are in RDI, RSI, RDX, RCX, R8, R9
	kuint64_t arg0 = regs->rdi;
	kuint64_t arg1 = regs->rsi;
	kuint64_t arg2 = regs->rdx;
	kuint64_t arg3 = regs->rcx;
	kuint64_t arg4 = regs->r8;
	kuint64_t arg5 = regs->r9;

	// Check if syscall is valid
	if (!syscall_is_valid(syscall_no)) {
		debug.printf(
		    "syscall", "error", "Invalid syscall number %llu\n", syscall_no);
		regs->rax = SYSCALL_INVALID;
		return;
	}

	// Get syscall entry
	const syscall_entry_t *entry = syscall_get_info(syscall_no);
	if (entry == KNULL) {
		debug.printf("syscall",
		             "error",
		             "Failed to get syscall info for %llu\n",
		             syscall_no);
		regs->rax = SYSCALL_ERROR;
		return;
	}

	// Log syscall for debugging (can be disabled in production)
	debug.printf(
	    "syscall", "notice", "Calling syscall %llu: %s\n", syscall_no, entry->name);

	// Call the syscall handler
	kuint64_t result = entry->handler(syscall_no, arg0, arg1, arg2, arg3, arg4, arg5);

	// Return result in RAX
	regs->rax = result;

	debug.printf(
	    "syscall", "success", "Syscall %llu returned %llu\n", syscall_no, result);
}

struct Syscalls syscalls = {
    .init     = syscall_init,
    .bind     = syscall_bind,
    .unbind   = syscall_unbind,
    .get_info = syscall_get_info,
    .is_valid = syscall_is_valid,
    .handler  = syscall_handler,
};

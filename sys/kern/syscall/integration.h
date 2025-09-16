// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/kern/syscall/integration.h
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#pragma once

#include "syscall.h"

#include <arch/amd64/idt.h>
#include <lib/kstdio/kstdint.h>

// Syscall subsystem status
typedef enum {
	SYSCALL_STATUS_UNINITIALIZED = 0,
	SYSCALL_STATUS_INITIALIZING  = 1,
	SYSCALL_STATUS_READY         = 2,
	SYSCALL_STATUS_ERROR         = 3
} syscall_status_t;

// Initialize the complete syscall infrastructure
// This function should be called during kernel initialization
void
    syscall_infrastructure_init (void);

// Shutdown the syscall infrastructure (for testing/cleanup)
void
    syscall_infrastructure_shutdown (void);

// Get the current status of the syscall subsystem
syscall_status_t
    syscall_get_status (void);

// Print syscall subsystem information
void
    syscall_print_info (void);

// Print all registered syscalls
void
    syscall_print_table (void);

// Validate syscall infrastructure integrity
kuint8_t
    syscall_validate_infrastructure (void);

// Enable/disable syscall logging (for debugging)
void
    syscall_set_logging (kuint8_t enabled);

// Get syscall statistics
typedef struct {
	kuint64_t total_calls;
	kuint64_t successful_calls;
	kuint64_t failed_calls;
	kuint64_t invalid_calls;
	kuint64_t most_used_syscall;
	kuint64_t most_used_count;
} syscall_stats_t;

void
    syscall_get_stats (syscall_stats_t *stats);

void
    syscall_reset_stats (void);

// Advanced syscall management
void
    syscall_enable_security_checks (kuint8_t enabled);

void
    syscall_set_max_calls_per_second (kuint64_t max_calls);

// Integration with other kernel subsystems
void
    syscall_integrate_with_scheduler (void);

void
    syscall_integrate_with_memory_manager (void);

void
    syscall_integrate_with_filesystem (void);

// Development and debugging utilities
void
    syscall_dump_registers (const registers_t *regs);

void
    syscall_trace_call (kuint64_t syscall_no,
                        kuint64_t arg0,
                        kuint64_t arg1,
                        kuint64_t arg2,
                        kuint64_t arg3,
                        kuint64_t arg4,
                        kuint64_t arg5);

// Quick test interface for kernel developers
void
    syscall_run_quick_test (void);

void
    syscall_benchmark (kuint32_t iterations);

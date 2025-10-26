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

#include "syscall.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/idt/idt.h>
#endif
#include <kstdint.h>

// Syscall subsystem status
typedef enum {
	SYSCALL_STATUS_UNINITIALIZED = 0,
	SYSCALL_STATUS_INITIALIZING  = 1,
	SYSCALL_STATUS_READY         = 2,
	SYSCALL_STATUS_ERROR         = 3
} syscall_status_t;

// Get syscall statistics
typedef struct {
	uint64_t total_calls;
	uint64_t successful_calls;
	uint64_t failed_calls;
	uint64_t invalid_calls;
	uint64_t most_used_syscall;
	uint64_t most_used_count;
} syscall_stats_t;

namespace syscall {
	namespace infrastructure {
		void             set_logging(uint8_t enabled);
		void             get_stats(syscall_stats_t *stats);
		uint8_t          validate(void);
		void             quick_test(void);
		void             toggle_security_checks(uint8_t enabled);
		void             set_max_calls_per_second(uint64_t max_calls);
		void             dump_registers(const registers_t *regs);
		void             trace_call(uint64_t syscall_no,
		                            uint64_t arg0,
		                            uint64_t arg1,
		                            uint64_t arg2,
		                            uint64_t arg3,
		                            uint64_t arg4,
		                            uint64_t arg5);
		void             reset_stats(void);
		void             print_info(void);
		void             print_table(void);
		void             shutdown(void);
		syscall_status_t get_status(void);
		void             init(void);

	}  // namespace infrastructure
}  // namespace syscall

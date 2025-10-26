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

#ifdef ARCH_AMD64
#	include <arch/amd64/isr/registers.h>
#endif
#include <kstdint.h>

extern struct Syscalls syscalls;

// Maximum number of syscalls supported
#define SYSCALL_MAX_COUNT 256

// Syscall interrupt number (int 0x80)
#define SYSCALL_INT_NO 0x80

// File operations
#define SYS_READ  0
#define SYS_WRITE 1
#define SYS_OPEN  2
#define SYS_CLOSE 3
#define SYS_STAT  4
#define SYS_FSTAT 5
#define SYS_LSTAT 6
#define SYS_POLL  7

// Memory management
#define SYS_BRK    12
#define SYS_MMAP   9
#define SYS_MUNMAP 11

// Process management
#define SYS_RT_SIGACTION   13
#define SYS_RT_SIGPROCMASK 14
#define SYS_IOCTL          16
#define SYS_GETPID         39
#define SYS_FORK           57
#define SYS_VFORK          58
#define SYS_EXECVE         59
#define SYS_EXIT           60
#define SYS_WAIT4          61

// Thread operations (for musl TLS)
#define SYS_ARCH_PRCTL      158
#define SYS_SET_TID_ADDRESS 218
#define SYS_EXIT_GROUP      231

// Time
#define SYS_TIME          201
#define SYS_CLOCK_GETTIME 228

// Syscall return values
#define SYSCALL_SUCCESS           0ULL
#define SYSCALL_ERROR             0xFFFFFFFFFFFFFFFFULL
#define SYSCALL_INVALID           0xFFFFFFFFFFFFFFFEULL
#define SYSCALL_NOT_IMPLEMENTED   0xFFFFFFFFFFFFFFFDULL
#define SYSCALL_PERMISSION_DENIED 0xFFFFFFFFFFFFFFFCULL
#define SYSCALL_INVALID_ARGS      0xFFFFFFFFFFFFFFFBULL

// Syscall handler function type
// Takes syscall number and up to 6 arguments, returns result
typedef uint64_t (*syscall_handler_t)(uint64_t syscall_no,
                                      uint64_t arg0,
                                      uint64_t arg1,
                                      uint64_t arg2,
                                      uint64_t arg3,
                                      uint64_t arg4,
                                      uint64_t arg5);

// Syscall table entry
typedef struct {
	syscall_handler_t handler;
	const char       *name;
	uint8_t           arg_count;  // Expected number of arguments
} syscall_entry_t;

namespace syscall {
	void             bind(uint64_t          syscall_no,
	                      syscall_handler_t handler,
	                      const char       *name,
	                      uint8_t           arg_count);
	void             unbind(uint64_t syscall_no);
	void             init(void);
	syscall_entry_t *get_info(uint64_t syscall_no);
	uint8_t          is_valid(uint64_t syscall_no);
	void             handler(registers_t *regs);
}  // namespace syscall

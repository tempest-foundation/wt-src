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

// System call numbers
#define SYS_EXIT     0   // Process exit
#define SYS_READ     1   // Read from file descriptor
#define SYS_WRITE    2   // Write to file descriptor
#define SYS_OPEN     3   // Open file
#define SYS_CLOSE    4   // Close file descriptor
#define SYS_GETPID   5   // Get process ID
#define SYS_GETPPID  6   // Get parent process ID
#define SYS_FORK     7   // Fork process
#define SYS_EXEC     8   // Execute program
#define SYS_WAIT     9   // Wait for child process
#define SYS_KILL     10  // Kill process
#define SYS_BRK      11  // Change data segment size
#define SYS_MMAP     12  // Map memory
#define SYS_MUNMAP   13  // Unmap memory
#define SYS_SLEEP    14  // Sleep for specified time
#define SYS_YIELD    15  // Yield CPU to other processes
#define SYS_TIME     16  // Get current time
#define SYS_GETCWD   17  // Get current working directory
#define SYS_CHDIR    18  // Change current directory
#define SYS_MKDIR    19  // Create directory
#define SYS_RMDIR    20  // Remove directory
#define SYS_STAT     21  // Get file statistics
#define SYS_LSTAT    22  // Get link statistics
#define SYS_FSTAT    23  // Get file descriptor statistics
#define SYS_LINK     24  // Create hard link
#define SYS_UNLINK   25  // Remove file
#define SYS_SYMLINK  26  // Create symbolic link
#define SYS_READLINK 27  // Read symbolic link
#define SYS_CHMOD    28  // Change file permissions
#define SYS_CHOWN    29  // Change file ownership
#define SYS_TRUNCATE 30  // Truncate file
#define SYS_LSEEK    31  // Seek in file

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
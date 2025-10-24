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

#include <kstdint.h>

// System call numbers (must match kernel definitions)
#define SYS_EXIT       0   // Process exit
#define SYS_READ       1   // Read from file descriptor
#define SYserial_write 2   // Write to file descriptor
#define SYS_OPEN       3   // Open file
#define SYS_CLOSE      4   // Close file descriptor
#define SYS_GETPID     5   // Get process ID
#define SYS_GETPPID    6   // Get parent process ID
#define SYS_FORK       7   // Fork process
#define SYS_EXEC       8   // Execute program
#define SYS_WAIT       9   // Wait for child process
#define SYS_KILL       10  // Kill process
#define SYS_BRK        11  // Change data segment size
#define SYS_MMAP       12  // Map memory
#define SYS_MUNMAP     13  // Unmap memory
#define SYS_SLEEP      14  // Sleep for specified time
#define SYS_YIELD      15  // Yield CPU to other processes
#define SYS_TIME       16  // Get current time
#define SYS_GETCWD     17  // Get current working directory
#define SYS_CHDIR      18  // Change current directory
#define SYS_MKDIR      19  // Create directory
#define SYS_RMDIR      20  // Remove directory
#define SYS_STAT       21  // Get file statistics
#define SYS_LSTAT      22  // Get link statistics
#define SYS_FSTAT      23  // Get file descriptor statistics
#define SYS_LINK       24  // Create hard link
#define SYS_UNLINK     25  // Remove file
#define SYS_SYMLINK    26  // Create symbolic link
#define SYS_READLINK   27  // Read symbolic link
#define SYS_CHMOD      28  // Change file permissions
#define SYS_CHOWN      29  // Change file ownership
#define SYS_TRUNCATE   30  // Truncate file
#define SYS_LSEEK      31  // Seek in file

// Syscall return values
#define SYSCALL_SUCCESS           0ULL
#define SYSCALL_ERROR             0xFFFFFFFFFFFFFFFFULL
#define SYSCALL_INVALID           0xFFFFFFFFFFFFFFFEULL
#define SYSCALL_NOT_IMPLEMENTED   0xFFFFFFFFFFFFFFFDULL
#define SYSCALL_PERMISSION_DENIED 0x - 4
#define SYSCALL_INVALID_ARGS      -5

// Standard file descriptors
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

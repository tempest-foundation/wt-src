// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/kern/syscall/syscalls.h
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

#include <lib/kstdio/kstdint.h>

// System call numbers (must match kernel definitions)
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
#define SYSCALL_PERMISSION_DENIED 0x - 4
#define SYSCALL_INVALID_ARGS      -5

// Standard file descriptors
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// Low-level syscall interface functions
// These perform the actual int 0x80 system calls

static inline kuint64_t
    syscall0 (kuint64_t syscall_no) {
	kuint64_t ret;
	__asm__ volatile("int $0x80" : "=a"(ret) : "a"(syscall_no) : "memory");
	return ret;
}

static inline kuint64_t
    syscall1 (kuint64_t syscall_no, kuint64_t arg0) {
	kuint64_t ret;
	__asm__ volatile("int $0x80" : "=a"(ret) : "a"(syscall_no), "D"(arg0) : "memory");
	return ret;
}

static inline kuint64_t
    syscall2 (kuint64_t syscall_no, kuint64_t arg0, kuint64_t arg1) {
	kuint64_t ret;
	__asm__ volatile("int $0x80"
	                 : "=a"(ret)
	                 : "a"(syscall_no), "D"(arg0), "S"(arg1)
	                 : "memory");
	return ret;
}

static inline kuint64_t
    syscall3 (kuint64_t syscall_no, kuint64_t arg0, kuint64_t arg1, kuint64_t arg2) {
	kuint64_t ret;
	__asm__ volatile("int $0x80"
	                 : "=a"(ret)
	                 : "a"(syscall_no), "D"(arg0), "S"(arg1), "d"(arg2)
	                 : "memory");
	return ret;
}

static inline kuint64_t
    syscall4 (kuint64_t syscall_no,
              kuint64_t arg0,
              kuint64_t arg1,
              kuint64_t arg2,
              kuint64_t arg3) {
	kuint64_t ret;
	__asm__ volatile("int $0x80"
	                 : "=a"(ret)
	                 : "a"(syscall_no), "D"(arg0), "S"(arg1), "d"(arg2), "c"(arg3)
	                 : "memory");
	return ret;
}

static inline kuint64_t
    syscall5 (kuint64_t syscall_no,
              kuint64_t arg0,
              kuint64_t arg1,
              kuint64_t arg2,
              kuint64_t arg3,
              kuint64_t arg4) {
	kuint64_t          ret;
	register kuint64_t r8_reg __asm__("r8") = arg4;
	__asm__ volatile(
	    "int $0x80"
	    : "=a"(ret)
	    : "a"(syscall_no), "D"(arg0), "S"(arg1), "d"(arg2), "c"(arg3), "r"(r8_reg)
	    : "memory");
	return ret;
}

static inline kuint64_t
    syscall6 (kuint64_t syscall_no,
              kuint64_t arg0,
              kuint64_t arg1,
              kuint64_t arg2,
              kuint64_t arg3,
              kuint64_t arg4,
              kuint64_t arg5) {
	kuint64_t          ret;
	register kuint64_t r8_reg __asm__("r8") = arg4;
	register kuint64_t r9_reg __asm__("r9") = arg5;
	__asm__ volatile("int $0x80"
	                 : "=a"(ret)
	                 : "a"(syscall_no),
	                   "D"(arg0),
	                   "S"(arg1),
	                   "d"(arg2),
	                   "c"(arg3),
	                   "r"(r8_reg),
	                   "r"(r9_reg)
	                 : "memory");
	return ret;
}

// High-level syscall wrapper functions
// These provide a more user-friendly interface

// Process management
static inline void
    exit (int status) {
	syscall1(SYS_EXIT, (kuint64_t) status);
}

static inline kuint64_t
    getpid (void) {
	return syscall0(SYS_GETPID);
}

static inline kuint64_t
    getppid (void) {
	return syscall0(SYS_GETPPID);
}

// File I/O
static inline kuint64_t
    read (int fd, void *buffer, kuint64_t count) {
	return syscall3(SYS_READ, (kuint64_t) fd, (kuint64_t) buffer, count);
}

static inline kuint64_t
    write (int fd, const void *buffer, kuint64_t count) {
	return syscall3(SYS_WRITE, (kuint64_t) fd, (kuint64_t) buffer, count);
}

static inline kuint64_t
    open (const char *pathname, int flags) {
	return syscall2(SYS_OPEN, (kuint64_t) pathname, (kuint64_t) flags);
}

static inline kuint64_t
    close (int fd) {
	return syscall1(SYS_CLOSE, (kuint64_t) fd);
}

// Memory management
static inline void *
    sbrk (kuint64_t increment) {
	return (void *) syscall1(SYS_BRK, increment);
}

// Process control
static inline kuint64_t
    fork (void) {
	return syscall0(SYS_FORK);
}

static inline kuint64_t
    exec (const char *pathname) {
	return syscall1(SYS_EXEC, (kuint64_t) pathname);
}

static inline kuint64_t
    wait (int *status) {
	return syscall1(SYS_WAIT, (kuint64_t) status);
}

static inline kuint64_t
    kill (kuint64_t pid, int signal) {
	return syscall2(SYS_KILL, pid, (kuint64_t) signal);
}

// Scheduling
static inline kuint64_t
    sleep (kuint64_t seconds) {
	return syscall1(SYS_SLEEP, seconds);
}

static inline kuint64_t
    yield (void) {
	return syscall0(SYS_YIELD);
}

// Time
static inline kuint64_t
    time (kuint64_t *tloc) {
	return syscall1(SYS_TIME, (kuint64_t) tloc);
}

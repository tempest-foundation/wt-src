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
#include <kstdint.h>

#include <dbg/logger.h>
#include <kern/proc/process.h>
#include <kern/syscall/syscall.h>

uint64_t sys_execve(uint64_t syscall_no,
                    uint64_t path_ptr,
                    uint64_t argv,
                    uint64_t envp,
                    uint64_t arg3,
                    uint64_t arg4,
                    uint64_t arg5) {
	(void) syscall_no;
	(void) argv;
	(void) envp;
	(void) arg3;
	(void) arg4;
	(void) arg5;

	const char *path = (const char *) path_ptr;
	logger::debug::printf("syscall", "info", "execve(%s) called\n", path);

	// TODO: Load ELF binary and replace current process

	return SYSCALL_NOT_IMPLEMENTED;
}
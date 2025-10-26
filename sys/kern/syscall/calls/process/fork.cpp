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

uint64_t
    sys_fork(uint64_t syscall_no,
             uint64_t arg0,
             uint64_t arg1,
             uint64_t arg2,
             uint64_t arg3,
             uint64_t arg4,
             uint64_t arg5) {
	(void) syscall_no;
	(void) arg0;
	(void) arg1;
	(void) arg2;
	(void) arg3;
	(void) arg4;
	(void) arg5;

	logger::debug::printf("syscall", "info", "fork() called\n");

	process_t *parent = proc::get_current_process();
	if( !parent ) {
		return SYSCALL_ERROR;
	}

	return SYSCALL_NOT_IMPLEMENTED;
}
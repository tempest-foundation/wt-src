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
#include "waitpid.hpp"

#include <kstdint.hpp>

#include <dbg/logger.hpp>
#include <kern/proc/process.hpp>
#include <kern/syscall/syscall.hpp>

uint64_t sys_waitpid(uint64_t syscall_no,
                     uint64_t pid,
                     uint64_t status_ptr,
                     uint64_t options,
                     uint64_t arg3,
                     uint64_t arg4,
                     uint64_t arg5) {
	(void) syscall_no;
	(void) pid;
	(void) status_ptr;
	(void) options;
	(void) arg3;
	(void) arg4;
	(void) arg5;

	logger::debug::printf("syscall", "info", "waitpid(%llu) called\n", pid);

	// TODO: Implement wait logic

	return SYSCALL_NOT_IMPLEMENTED;
}
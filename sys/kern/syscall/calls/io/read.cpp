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
#include <kern/syscall/syscall.h>

uint64_t
    sys_read(uint64_t syscall_no __attribute__((unused)),
             uint64_t fd,
             uint64_t buffer,
             uint64_t count,
             uint64_t arg3 __attribute__((unused)),
             uint64_t arg4 __attribute__((unused)),
             uint64_t arg5 __attribute__((unused))) {
	logger::debug::printf("syscall",
	                      "info",
	                      " read: fd=%llu, buffer=0x%llx, count=%llu\n",
	                      fd,
	                      buffer,
	                      count);
	// TODO: Implement actual file reading
	// Basic validation
	if( buffer == 0 || count == 0 )
		return SYSCALL_INVALID_ARGS;

	return SYSCALL_NOT_IMPLEMENTED;
}

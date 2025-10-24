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
    sys_write(uint64_t syscall_no __attribute__((unused)),
              uint64_t fd,
              uint64_t buffer,
              uint64_t count,
              uint64_t arg3 __attribute__((unused)),
              uint64_t arg4 __attribute__((unused)),
              uint64_t arg5 __attribute__((unused))) {
	logger::printf("syscall",
	               "info",
	               "write: fd=%llu, buffer=0x%llx, count=%llu\n",
	               fd,
	               buffer,
	               count);
	// TODO: Implement actual file writing
	// Basic validation
	if( buffer == 0 || count == 0 ) {
		return SYSCALL_INVALID_ARGS;
	}

	// For stdout (fd=1), we could implement a simple write to console
	if( fd == 1 ) {
		// TODO: Copy data from user buffer and write to console
		logger::printf("syscall", "info", "Write to stdout requested\n");
		return count;  // Pretend we wrote all bytes
	}

	return SYSCALL_NOT_IMPLEMENTED;
}

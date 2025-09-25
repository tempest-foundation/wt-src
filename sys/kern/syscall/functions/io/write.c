// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/kern/syscall/functions/io/write.c
 * Author(s):   Tempik25 <tempik25@tempestfoundation.org>
 *              (https://github.com/tempik25)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <debug/debug.h>
#include <kern/syscall/syscall.h>
#include <lib/kstdio/kstdint.h>

kuint64_t
    sys_write (kuint64_t syscall_no __attribute__((unused)),
               kuint64_t fd,
               kuint64_t buffer,
               kuint64_t count,
               kuint64_t arg3 __attribute__((unused)),
               kuint64_t arg4 __attribute__((unused)),
               kuint64_t arg5 __attribute__((unused))) {
	debug.printf(
	    "syscall", "write: fd=%llu, buffer=0x%llx, count=%llu\n", fd, buffer, count);
	// TODO: Implement actual file writing
	// Basic validation
	if (buffer == 0 || count == 0) {
		return SYSCALL_INVALID_ARGS;
	}

	// For stdout (fd=1), we could implement a simple write to console
	if (fd == 1) {
		// TODO: Copy data from user buffer and write to console
		debug.printf("syscall", "Write to stdout requested\n");
		return count;  // Pretend we wrote all bytes
	}

	return SYSCALL_NOT_IMPLEMENTED;
}

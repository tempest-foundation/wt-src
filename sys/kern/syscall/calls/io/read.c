// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/kern/syscall/calls/io/read.c
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
    sys_read (kuint64_t syscall_no __attribute__((unused)),
              kuint64_t fd,
              kuint64_t buffer,
              kuint64_t count,
              kuint64_t arg3 __attribute__((unused)),
              kuint64_t arg4 __attribute__((unused)),
              kuint64_t arg5 __attribute__((unused))) {
	debug.printf("syscall",
	             "info",
	             " read: fd=%llu, buffer=0x%llx, count=%llu\n",
	             fd,
	             buffer,
	             count);
	// TODO: Implement actual file reading
	// Basic validation
	if (buffer == 0 || count == 0) {
		return SYSCALL_INVALID_ARGS;
	}
	return SYSCALL_NOT_IMPLEMENTED;
}

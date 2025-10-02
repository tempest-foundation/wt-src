// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/syscall/calls/process/exit.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstdint.h>

#include <dbg/logger.h>
#include <kern/syscall/syscall.h>

kuint64_t
    sys_exit (kuint64_t syscall_no __attribute__((unused)),
              kuint64_t status,
              kuint64_t arg1 __attribute__((unused)),
              kuint64_t arg2 __attribute__((unused)),
              kuint64_t arg3 __attribute__((unused)),
              kuint64_t arg4 __attribute__((unused)),
              kuint64_t arg5 __attribute__((unused))) {
	logger.printf("syscall", "info", "Process exit with status %llu\n", status);
	// TODO: Implement actual process termination
	return SYSCALL_NOT_IMPLEMENTED;
}

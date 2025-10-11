// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/syscall/calls/process/getpid.c
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
    sys_getpid (kuint64_t syscall_no __attribute__((unused)),
                kuint64_t arg0 __attribute__((unused)),
                kuint64_t arg1 __attribute__((unused)),
                kuint64_t arg2 __attribute__((unused)),
                kuint64_t arg3 __attribute__((unused)),
                kuint64_t arg4 __attribute__((unused)),
                kuint64_t arg5 __attribute__((unused))) {
	// TODO: Implement actual process ID retrieval
	// For now, return a dummy PID
	kuint64_t pid = 1;
	logger.printf("syscall", "info", "getpid returning PID %llu\n", pid);
	return pid;
}

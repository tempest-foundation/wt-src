// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/syscall/calls/io/write.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kern/syscall/syscall.h>
#include <lib/kstdio/kstdint.h>

kuint64_t
    sys_write (kuint64_t syscall_no __attribute__((unused)),
               kuint64_t fd,
               kuint64_t buffer,
               kuint64_t count,
               kuint64_t arg3 __attribute__((unused)),
               kuint64_t arg4 __attribute__((unused)),
               kuint64_t arg5 __attribute__((unused)));

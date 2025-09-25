// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/kern/syscall/functions/process/getpid.h
 * Author(s):   Tempik25 <tempik25@tempestfoundation.org>
 *              (https://github.com/tempik25)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <kern/syscall/syscall.h>
#include <lib/kstdio/kstdint.h>

kuint64_t
    sys_getpid (kuint64_t syscall_no __attribute__((unused)),
                kuint64_t arg0 __attribute__((unused)),
                kuint64_t arg1 __attribute__((unused)),
                kuint64_t arg2 __attribute__((unused)),
                kuint64_t arg3 __attribute__((unused)),
                kuint64_t arg4 __attribute__((unused)),
                kuint64_t arg5 __attribute__((unused)));

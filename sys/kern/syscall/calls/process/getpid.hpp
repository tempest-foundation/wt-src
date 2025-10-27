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
#include <kstdint.hpp>

#include <kern/syscall/syscall.hpp>

uint64_t sys_getpid(uint64_t syscall_no __attribute__((unused)),
                    uint64_t arg0 __attribute__((unused)),
                    uint64_t arg1 __attribute__((unused)),
                    uint64_t arg2 __attribute__((unused)),
                    uint64_t arg3 __attribute__((unused)),
                    uint64_t arg4 __attribute__((unused)),
                    uint64_t arg5 __attribute__((unused)));

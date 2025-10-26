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
#pragma once

#include <kstdint.h>

#include <kern/syscall/syscall.h>

uint64_t
    sys_brk(uint64_t syscall_no,
            uint64_t addr,
            uint64_t arg1,
            uint64_t arg2,
            uint64_t arg3,
            uint64_t arg4,
            uint64_t arg5);

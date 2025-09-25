// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/kern/framebuf/framebuf.c
 * Author(s):   Tempik25 <tempik25@tempestfoundation.org>
 *              (https://github.com/tempik25)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include "framebuf.h"

#include <debug/debug.h>
#include <lib/kstdio/kstdint.h>

struct framebuffer_info fb_info;

void
    map_framebuffer_address (kuint64_t phys_addr) {
	kuint64_t virt_addr = 0xFFFF800000000000ULL + phys_addr;

	debug.printf(
	    "fb", "info", "Mapping framebuffer 0x%llx -> 0x%llx\n", phys_addr, virt_addr);

	fb_info.addr = virt_addr;
}

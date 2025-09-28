// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/init/main.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "main.h"

#include <arch/amd64/cpuid.h>
#include <arch/amd64/idt.h>
#include <dbg/logger.h>
#include <drv/driver.h>
#include <fs/ext2/ext2.h>
#include <kern/framebuf/framebuf.h>
#include <kern/mb/mb.h>
#include <kern/memory/memory.h>
#include <kern/syscall/integration.h>
#include <lib/kstdio/kprint/kprint.h>
#include <shell/shell.h>

void
    start_kernel (void *mb_info) {
	idt_init();
	serial.init();

	if (mb_info == KNULL)
		__asm__("cli; hlt");

	parse_multiboot_info(mb_info);
	video.init(&fb_info);

	kmemory_init(mb_info);
	init_memory_pools();

	// Initialize syscall infrastructure
	syscall_infrastructure_init();

	ext2.set_block_device(ata.pio_read, KNULL);
	if (ext2.mount(0) != 0)
		logger.err("fs", "EXT2 mount failed", KNULL);

	kcpu_init_brand();
	keyboard.init();

	__asm__ volatile("sti");

	kshell();
}

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
#ifdef ARCH_AMD64
#	include <arch/amd64/cpu/cpuid.h>
#	include <arch/amd64/idt/idt.h>
#endif
#include <kprint.h>

#include <dbg/logger.h>
#include <drv/driver.h>
#include <drv/tty/tty.h>
#include <fs/ext2/ext2.h>
#include <kern/framebuf/framebuf.h>
#include <kern/mb/mb.h>
#include <kern/memory/memory.h>
#include <kern/syscall/integration.h>
#include <kshell/kernSh.h>

/**
 * @brief Entry point for the kernel initialization sequence.
 *
 * This function is the primary entry of the Tempest kernel and performs the core system
 * initialization tasks required before handing off to the kernel shell.
 * It performs the following:
 * - Initializes the Interrupt Descriptor Table (IDT)
 * - Initializes the serial interface
 * - Checks for valid multiboot information and halts if absent
 * - Parses the provided multiboot information
 * - Initializes the framebuffer/video output
 * - Sets up the memory manager and memory pools
 * - Initializes system call infrastructure
 * - Mounts the EXT2 filesystem
 * - Initializes the CPU brand string
 * - Sets up keyboard input
 * - Enables interrupts
 * - Launches the kernel shell (placeholder)
 *
 * @param mb_info Pointer to the multiboot information structure provided at boot time.
 *                If @p mb_info is NULL, the system will halt.
 */
extern "C" void
    start_kernel(void *mb_info) {
	idt_init();
	serial::init();

	if( mb_info == nullptr )
		__asm__("cli; hlt");

	parse_multiboot_info(mb_info);
	video::init(&fb_info);

	memory::init(mb_info);
	memory::pool::init_memory();

	// Initialize syscall infrastructure
	syscall::infrastructure::init();

	ext2::set_block_device(ata::pio_read, nullptr);
	if( ext2::mount(0) != 0 )
		logger::error("fs", "EXT2 mount failed", nullptr);

	cpu_init();

	__asm__ volatile("sti");

	tty::init();
	kshell();
}

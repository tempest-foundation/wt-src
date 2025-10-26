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
#	include <arch/amd64/cpu/instr/instr.h>
#	include <arch/amd64/idt/idt.h>
#	include <arch/amd64/cpu/halt.h>
#endif
#include <kprint.h>

#include <dbg/logger.h>
#include <drv/driver.h>
#include <drv/tty/tty.h>
#include <fs/ext2/ext2.h>
#include <kern/framebuf/framebuf.h>
#include <kern/loader/elf_loader.h>
#include <kern/multiboot/multiboot.h>
#include <kern/memory/memory.h>
#include <kern/proc/process.h>
#include <kern/scheduler/scheduler.h>
#include <kern/syscall/integration.h>
#include <kern/timer/timer.h>
#include <kshell/kernSh.h>

static void
    isHardware_minReq(void) {
#ifdef ARCH_AMD64
	/*
	 * Well, you are asking: "Where is the fallback in the case the system dosen't have SSE or FPU?".
	 * So, the anwser is... "Fuck you and get a new CPU!".  Wind/Tempest will simply NOT support
	 * systems without a FPU and SSE, because the system is modern and if you are using Wind/Tempest
	 * from a old system (that is basically very difficuly to get a CPU without FPU and SSE in 64 bits)
	 * you will simply not use it.
	 */
	if( !amd64::cpuid::instr::has_fpu() && !amd64::cpuid::instr::has_sse2() ) {
		serial::writes(
		    "No FPU and SSE2 detected.  This CPU is from the dinossour age.");
		amd64::halt();
	}

	if( !amd64::cpuid::instr::has_sse2() ) {
		serial::writes(
		    "No SSE2 detected.  Seriously, go buy a new CPU, if not, fuck you!");
		amd64::halt();
	}

	if( !amd64::cpuid::instr::has_fpu() ) {
		serial::writes(
		    "No FPU detected.  Seriously, go buy a new CPU, if not, fuck you!");
		amd64::halt();
	}
#endif
}

extern "C" void
    load_gdt(void);

extern "C" void
    enter_userspace(uint64_t rip, uint64_t rsp);

/**
 * @brief Entry point for the kernel initialization sequence.
 *
 * This function is the primary entry of the Tempest kernel and performs the core system
 * initialization tasks required before handing off to the kernel shell (placeholder).
 *
 * @param mb_info Pointer to the multiboot information structure provided at boot time.
 *                If @p mb_info is NULL, the system will halt.
 */
extern "C" void
    start_kernel(void *mb_info) {
	serial::init();

	isHardware_minReq();

	load_gdt();
#ifdef ARCH_AMD64
	amd64::idt::init();
#endif
	timer_init();  // Initialize the uptime timer

	multiboot::parse(mb_info);
	video::init(&fb_info);
	tty::init();

	memory::init(mb_info);
	memory::pool::init_memory();

	// Initialize syscall infrastructure
	syscall::infrastructure::init();
	// Initialize processes subsystem
	proc::init();

	// Initalize EXT2
	ext2::set_block_device(ata::pio_read, nullptr);
	if( ext2::mount(0) != 0 )
		logger::error("fs", "EXT2 mount failed\n", nullptr);

#ifdef ARCH_AMD64
	amd64::cpuid::init();  // Initialize the vendor and brand of the CPU
#endif

	__asm__ volatile("sti");

	kshell::init();
}

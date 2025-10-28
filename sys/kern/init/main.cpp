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
#	include <arch/amd64/cpu/cpuid.hpp>
#	include <arch/amd64/cpu/instr/instr.hpp>
#	include <arch/amd64/idt/idt.hpp>
#	include <arch/amd64/cpu/halt.hpp>
#endif

#include <kprint.hpp>
#include <kutoa.hpp>

#include <dbg/logger.hpp>
#include <drv/driver.hpp>
#include <drv/serial/serial.hpp>
#include <drv/tty/tty.hpp>
#include <fs/ext2/ext2.hpp>
#include <kern/framebuf/framebuf.hpp>
#include <kern/loader/elf_loader.hpp>
#include <kern/memory/memory.hpp>
#include <kern/multiboot/multiboot.hpp>
#include <kern/proc/process.hpp>
#include <kern/scheduler/scheduler.hpp>
#include <kern/syscall/integration.hpp>
#include <kern/timer/timer.hpp>

static void isHardware_minReq(void) {
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

extern "C" void load_gdt(void);
extern "C" void set_kernel_stack(uint64_t rsp);
extern "C" void enter_userspace(uint64_t rip, uint64_t rsp);

/**
 * @brief Entry point for the kernel initialization sequence.
 *
 * This function is the primary entry of the Tempest kernel and performs the core system
 * initialization tasks required before handing off to the kernel shell (placeholder).
 *
 * @param mb_info Pointer to the multiboot information structure provided at boot time.
 *                If @p mb_info is NULL, the system will halt.
 */
extern "C" void start_kernel(void *mb_info) {
	serial::init();

	isHardware_minReq();

	load_gdt();
	
	// Set TSS.RSP0 immediately after loading GDT
	// This ensures exceptions from userspace can be handled properly
	uint64_t kernel_stack;
	__asm__ volatile("mov %%rsp, %0" : "=r"(kernel_stack));
	set_kernel_stack(kernel_stack);
	
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

	__asm__ volatile("sti");  // Enables interrupts after initialization is complete

	// Load and execute init program
	const char *init_path = "/System/boot/init";
	ext2_file_t init_file;

	logger::info("init", "Loading init program", init_path);

	if( ext2::open(init_path, &init_file) != 0 ) {
		logger::error("init", "Failed to open init program\n", nullptr);
		amd64::halt();
	}

	// Get file size from inode
	size_t init_size = init_file.inode.size_lo;
	logger::debug::printf(
	    "init", "info", "Init program size: %llu bytes\n", (uint64_t) init_size);

	// Validate file size
	if( init_size == 0 || init_size > (64 * 1024 * 1024) ) {
		logger::error("init", "Invalid init program size\n", nullptr);
		amd64::halt();
	}

	// Allocate buffer for init binary
	void *init_buffer = memory::malloc(init_size);
	if( !init_buffer ) {
		logger::error("init", "Failed to allocate buffer for init\n", nullptr);
		amd64::halt();
	}

	// Read init binary
	if( ext2::read(&init_file, init_buffer, init_size) != (int) init_size ) {
		logger::error("init", "Failed to read init binary\n", nullptr);
		memory::free(init_buffer);
		amd64::halt();
	}

	// Load ELF and get entry point
	uint64_t entry_point = 0;
	if( elf_loader::load_elf(init_buffer, init_size, &entry_point) != 0 ) {
		logger::error("init", "Failed to load init ELF\n", nullptr);
		memory::free(init_buffer);
		amd64::halt();
	}
	
	// Free init buffer now that ELF is loaded into memory
	memory::free(init_buffer);

	// Set up user stack (allocate and map stack pages)
	// Use address within first 4GB since bootloader only identity-maps 0-4GB
	uint64_t user_stack_top  = 0x7FFFF000ULL;  // Stack at ~128MB (within 4GB)
	uint64_t stack_size      = 8 * PAGE_SIZE;  // 32KB stack
	uint64_t user_stack_base = user_stack_top - stack_size;

	// Allocate and map stack pages
	for( uint64_t addr = user_stack_base; addr < user_stack_top; addr += PAGE_SIZE ) {
		page_frame_t *frame = memory::allocate_page_frame();
		if( !frame ) {
			logger::error("init", "Failed to allocate stack page\n", nullptr);
			amd64::halt();
		}

		uint64_t phys_addr = memory::get_physical_addr(frame);
		uint64_t flags     = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
		memory::vm::map_page(addr, phys_addr, flags);
	}
	
	// Enter userspace (TSS.RSP0 was set early in boot, exceptions will work)
	enter_userspace(entry_point, user_stack_top);

	// Should never reach here
	serial::writes("ERROR: Returned from userspace!\n");
	for( ;; )
		__asm__ volatile("hlt");
}

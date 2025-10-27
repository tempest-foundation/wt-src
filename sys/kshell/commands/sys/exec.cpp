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
#include "exec.h"

#include <kprint.h>
#include <kstring.h>

#include <fs/ext2/ext2.h>
#include <fs/vfs/vfs.h>
#include <kern/loader/elf_loader.h>
#include <kern/memory/memory.h>
#include <kern/proc/process.h>
#include <kern/scheduler/scheduler.h>

extern "C" void enter_userspace(uint64_t rip, uint64_t rsp);

// Maximum ELF file size we'll attempt to load (4MB)
#define MAX_ELF_SIZE (4 * 1024 * 1024)

void cmd_exec(const char *args) {
	if( !args || *args == '\0' ) {
		kstd::puts("Usage: exec <elf_path>\n");
		kstd::puts("  Loads and executes an ELF binary in user-space\n");
		return;
	}

	// Resolve path
	char        abs_path[256];
	const char *path = args;
	if( path[0] != '/' ) {
		vfs::resolve(path, abs_path, sizeof(abs_path));
		path = abs_path;
	}

	kstd::printf("Loading ELF file: %s\n", path);

	// Open the ELF file
	ext2_file_t file;
	int         rc = ext2::open(path, &file);
	if( rc != 0 ) {
		kstd::printf("exec: cannot open %s (error %d)\n", path, rc);
		return;
	}

	// Get file size from inode
	uint32_t file_size = file.inode.size_lo;

	if( file_size == 0 ) {
		kstd::puts("exec: file is empty\n");
		return;
	}

	if( file_size > MAX_ELF_SIZE ) {
		kstd::printf(
		    "exec: file too large (%u bytes, max %u)\n", file_size, MAX_ELF_SIZE);
		return;
	}

	kstd::printf("File size: %u bytes\n", file_size);

	// Allocate buffer for ELF file
	void *elf_buffer = memory::malloc(file_size);
	if( !elf_buffer ) {
		kstd::puts("exec: failed to allocate memory for ELF file\n");
		return;
	}

	// Read entire file into buffer
	int bytes_read = ext2::read(&file, (char *) elf_buffer, file_size);
	if( bytes_read != (int) file_size ) {
		kstd::printf("exec: failed to read file (read %d of %u bytes)\n",
		             bytes_read,
		             file_size);
		memory::free(elf_buffer);
		return;
	}

	// Validate ELF file
	if( !elf_loader::is_valid_elf(elf_buffer) ) {
		kstd::puts("exec: not a valid ELF file\n");
		memory::free(elf_buffer);
		return;
	}

	kstd::puts("ELF validation: OK\n");

	// Create a new process for this program
	// Note: We pass a dummy entry point, will update it after loading
	process_t *proc = proc::create_process((void (*)()) 0x400000, false);
	if( !proc ) {
		kstd::puts("exec: failed to create process\n");
		memory::free(elf_buffer);
		return;
	}

	kstd::printf("Created process PID=%u\n", proc->pid);

	// Save current page table (kernel)
	uint64_t kernel_cr3;
	__asm__ volatile("mov %%cr3, %0" : "=r"(kernel_cr3));

	// Switch to process page table to load ELF into its address space
	uint64_t proc_cr3 = (uint64_t) proc->page_table;
	__asm__ volatile("mov %0, %%cr3" : : "r"(proc_cr3) : "memory");

	// Load ELF into the process's address space
	uint64_t entry_point = 0;
	rc                   = elf_loader::load_elf(elf_buffer, file_size, &entry_point);

	// Switch back to kernel page table
	__asm__ volatile("mov %0, %%cr3" : : "r"(kernel_cr3) : "memory");

	// Free the ELF buffer
	memory::free(elf_buffer);

	if( rc != 0 ) {
		kstd::puts("exec: failed to load ELF\n");
		proc->state = PROC_STATE_UNUSED;
		return;
	}

	kstd::printf("ELF loaded successfully\n");
	kstd::printf("Entry point: 0x%llx\n", entry_point);
	kstd::printf("User stack: 0x%llx\n", proc->rsp);

	// Update process entry point
	proc->rip = entry_point;

	kstd::puts("\n=== Entering User-Space ===\n");
	kstd::printf("RIP: 0x%llx\n", proc->rip);
	kstd::printf("RSP: 0x%llx\n", proc->rsp);
	kstd::puts("============================\n\n");

	// Switch to the process's page table
	__asm__ volatile("mov %0, %%cr3" : : "r"(proc_cr3) : "memory");

	// Jump to user-space!
	// This will set up the proper ring 3 segments and jump to the entry point
	enter_userspace(proc->rip, proc->rsp);

	// Should never reach here (unless the program exits via syscall)
	kstd::puts("Returned from user-space\n");
}
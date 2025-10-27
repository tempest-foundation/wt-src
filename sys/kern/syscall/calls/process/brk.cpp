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
// SPDX-License-Identifier: GPL-3.0-only
/*
 * sys/kern/syscall/calls/process/fork.cpp
 * Process management syscalls
 */
#include "brk.hpp"

#include <kstdint.hpp>

#include <dbg/logger.hpp>
#include <kern/proc/process.hpp>
#include <kern/syscall/syscall.hpp>

uint64_t sys_brk(uint64_t syscall_no,
                 uint64_t addr,
                 uint64_t arg1,
                 uint64_t arg2,
                 uint64_t arg3,
                 uint64_t arg4,
                 uint64_t arg5) {
	(void) syscall_no;
	(void) arg1;
	(void) arg2;
	(void) arg3;
	(void) arg4;
	(void) arg5;

	process_t *proc = proc::get_current_process();
	if( !proc ) {
		return SYSCALL_ERROR;
	}

	if( addr == 0 ) {
		// Return current heap end
		return proc->heap_end;
	}

	// Expand heap
	if( addr > proc->heap_end ) {
		// Allocate new pages
		uint64_t old_end = proc->heap_end;
		uint64_t new_end = addr;

		for( uint64_t va = (old_end + PAGE_SIZE - 1) & (uint64_t) ~0xFFF;
		     va < new_end;
		     va += PAGE_SIZE ) {
			page_frame_t *frame = memory::allocate_page_frame();
			if( !frame ) {
				return proc->heap_end;  // Return old value on failure
			}

			uint64_t pa = memory::get_physical_addr(frame);
			memory::vm::map_page(
			    va, pa, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
		}

		proc->heap_end = new_end;
	}

	return proc->heap_end;
}
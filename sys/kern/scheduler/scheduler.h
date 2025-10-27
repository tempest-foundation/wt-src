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

#ifdef ARCH_AMD64
#	include <arch/amd64/idt/idt.h>
#endif
#include <kern/proc/process.h>

namespace scheduler {
	void       init(void);
	void       add_process(process_t *proc);
	void       remove_process(process_t *proc);
	void       schedule(void);
	void       tick(void);  // Call from timer interrupt
	process_t *pick_next(void);
	process_t *get_current_process(void);

	// Context switching functions
	void save_context(process_t *proc, registers_t *regs);
	void restore_context(process_t *proc, registers_t *regs);
}  // namespace scheduler

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
#include <kstdint.h>

namespace amd64::fpu {
	void enable(void) {
		// Enable FPU
		uint64_t cr0;
		__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
		cr0 &= ~(uint64_t(1) << 2);  // Clear EM (emulation) bit
		cr0 |= (uint64_t(1) << 1);   // Set MP (monitor coprocessor) bit
		__asm__ volatile("mov %0, %%cr0" : : "r"(cr0));

		// Initialize FPU
		__asm__ volatile("fninit");
	}
}  // namespace amd64::fpu

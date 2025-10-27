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

namespace amd64::sse2 {
	void enable(void) {
		// Enable SSE 2
		uint64_t cr4;
		__asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
		cr4 |= (3 << 9);  // Set OSFXSR and OSXMMEXCPT bits
		__asm__ volatile("mov %0, %%cr4" : : "r"(cr4));
	}
}  // namespace amd64::sse2

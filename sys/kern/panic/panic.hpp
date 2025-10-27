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
#	include <arch/amd64/isr/registers.hpp>
#endif

namespace panic {
	void dump_registers(registers_t *r);
	void init(int code, registers_t *regs);
}  // namespace panic

enum panic_code {
	PANIC_UNKNOWN_ERROR = 0,
	PANIC_DIVISION_BY_ZERO,
	PANIC_DOUBLE_FAULT,
	PANIC_GENERAL_PROTECTION,
	PANIC_PAGE_FAULT,
	PANIC_STACK_SEGMENT,
	PANIC_SEGMENT_NOT_PRESENT,
	PANIC_INVALID_TSS,
	PANIC_ALIGNMENT_CHECK,
	PANIC_MACHINE_CHECK,
	PANIC_SIMD_EXCEPTION,
	PANIC_VIRTUALIZATION,
	PANIC_CONTROL_PROTECTION,
	PANIC_HYPERVISOR,
	PANIC_VMM_COMMUNICATION,
	PANIC_SECURITY,
	PANIC_INVALID_OPCODE
};

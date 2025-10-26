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

#include <arch/amd64/isr/registers.h>

typedef void (*irq_handler_t)(registers_t *);

// Syscall support
extern "C" void
    syscall_int_handler(void);

namespace amd64 {
	namespace irq {
		void bind(int irq, irq_handler_t handler);
	}

	namespace idt {
		void setup_syscall(void);
		void init(void);
	}  // namespace idt
}  // namespace amd64
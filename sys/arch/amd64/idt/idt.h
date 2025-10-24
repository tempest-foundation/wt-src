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

void
    idt_init(void);

typedef void (*irq_handler_t)(registers_t *);
void
    register_irq_handler(int irq, irq_handler_t handler);

// Syscall support
extern "C" void
    syscall_int_handler(void);
void
    idt_setup_syscall(void);

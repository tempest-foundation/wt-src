// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/arch/amd64/idt/idt.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <arch/amd64/isr/registers.h>

void
    idt_init (void);

typedef void (*irq_handler_t)(registers_t *);
void
    register_irq_handler (int irq, irq_handler_t handler);

// Syscall support
extern void
    syscall_int_handler (void);
void
    idt_setup_syscall (void);

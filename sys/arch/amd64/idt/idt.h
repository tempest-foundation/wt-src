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

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
#include "idt.hpp"

#include <kstdio.hpp>
#include <kutoa.hpp>

#include <arch/amd64/asm/io.hpp>
#include <dbg/logger.hpp>
#include <drv/serial/serial.hpp>
#include <kern/panic/panic.hpp>
#include <ktime.hpp>

// PIC (Programmable Interrupt Controller) ports.
#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20  // End-of-Interrupt command.

// IDT Constants
namespace idt_constants {
	constexpr uint8_t GATE_PRESENT        = 0x80;
	constexpr uint8_t GATE_DPL_KERNEL     = 0x00;
	constexpr uint8_t GATE_DPL_USER       = 0x60;
	constexpr uint8_t GATE_TYPE_INTERRUPT = 0x0E;

	constexpr uint8_t GATE_KERNEL_INTERRUPT =
	    GATE_PRESENT | GATE_DPL_KERNEL | GATE_TYPE_INTERRUPT;  // 0x8E
	constexpr uint8_t GATE_USER_INTERRUPT =
	    GATE_PRESENT | GATE_DPL_USER | GATE_TYPE_INTERRUPT;  // 0xEE

	constexpr uint16_t KERNEL_CODE_SELECTOR = 0x08;
	constexpr uint8_t  SYSCALL_VECTOR       = 0x80;
	constexpr uint8_t  IRQ_BASE             = 32;
}  // namespace idt_constants

extern "C" {
void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

void syscall_int_handler();
}

// Array of C-level interrupt handlers.
static irq_handler_t irq_handlers[16];

// IDT entry structure.
struct idt_entry {
	uint16_t base_lo;
	uint16_t sel;
	uint8_t  ist;
	uint8_t  flags;
	uint16_t base_mid;
	uint32_t base_hi;
	uint32_t reserved;
} __attribute__((packed));

// IDT pointer structure.
struct idt_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

static struct idt_entry Idt_entry[256];
static struct idt_ptr   idtp;

// Static lookup table for interrupt-to-panic mapping
static const struct {
	uint8_t interrupt_num;
	int     panic_code;
} isr_panic_map[] = {{0, PANIC_DIVISION_BY_ZERO},
                     {6, PANIC_INVALID_OPCODE},
                     {8, PANIC_DOUBLE_FAULT},
                     {10, PANIC_INVALID_TSS},
                     {11, PANIC_SEGMENT_NOT_PRESENT},
                     {12, PANIC_STACK_SEGMENT},
                     {13, PANIC_GENERAL_PROTECTION},
                     {14, PANIC_PAGE_FAULT},
                     {17, PANIC_ALIGNMENT_CHECK},
                     {18, PANIC_MACHINE_CHECK},
                     {19, PANIC_SIMD_EXCEPTION},
                     {20, PANIC_VIRTUALIZATION},
                     {21, PANIC_CONTROL_PROTECTION},
                     {28, PANIC_HYPERVISOR},
                     {29, PANIC_VMM_COMMUNICATION},
                     {30, PANIC_SECURITY}};

static int get_panic_code_for_interrupt(uint8_t int_no) {
	constexpr size_t map_size = sizeof(isr_panic_map) / sizeof(isr_panic_map[0]);

	for( size_t i = 0; i < map_size; ++i ) {
		if( isr_panic_map[i].interrupt_num == int_no ) {
			return isr_panic_map[i].panic_code;
		}
	}

	return PANIC_UNKNOWN_ERROR;
}

extern "C" void isr_handler(registers_t *regs) {
	// Special handling for page faults - log details before panicking
	if( regs->int_no == 14 ) {
		uint64_t cr2;
		__asm__ volatile("mov %%cr2, %0" : "=r"(cr2));

		char buf[32];

		serial::writes("[PAGE FAULT] addr=0x");
		kstd::utoa(buf, buf + 32, cr2, 16, 16);
		serial::writes(buf);
		serial::writes(" err=0x");
		kstd::utoa(buf, buf + 32, regs->err_code, 16, 1);
		serial::writes(buf);
		
		// Decode error code
		serial::writes(" (");
		if (regs->err_code & 0x1) serial::writes("PRESENT ");
		else serial::writes("NOT-PRESENT ");
		if (regs->err_code & 0x2) serial::writes("WRITE ");
		else serial::writes("READ ");
		if (regs->err_code & 0x4) serial::writes("USER");
		else serial::writes("KERNEL");
		serial::writes(")\n");
		
		// Now panic with details
		int panic_code = get_panic_code_for_interrupt((uint8_t) regs->int_no);
		panic::init(panic_code, regs);
		return;
	}

	int panic_code = get_panic_code_for_interrupt((uint8_t) regs->int_no);
	panic::init(panic_code, regs);
}

extern "C" void irq_handler(registers_t *regs) {
	// If a custom handler is registered, call it.
	if( regs->int_no >= idt_constants::IRQ_BASE && regs->int_no <= 47 ) {
		irq_handler_t handler =
		    irq_handlers[regs->int_no - idt_constants::IRQ_BASE];
		if( handler ) {
			handler(regs);
		}
	}

	// Send End-of-Interrupt (EOI) to the PICs.
	if( regs->int_no >= 40 ) {
		outb(PIC2_CMD, PIC_EOI);  // EOI to slave PIC.
	}
	outb(PIC1_CMD, PIC_EOI);  // EOI to master PIC.
}

// Remap the PIC to avoid conflicts with CPU exceptions
static void pic_remap(uint8_t master_offset, uint8_t slave_offset) {
	outb(PIC1_CMD, 0x11);
	outb(PIC2_CMD, 0x11);
	outb(PIC1_DATA, master_offset);  // Master PIC vector offset.
	outb(PIC2_DATA, slave_offset);   // Slave PIC vector offset.
	outb(PIC1_DATA, 0x04);           // Tell Master PIC there is a slave PIC at IRQ2.
	outb(PIC2_DATA, 0x02);           // Tell Slave PIC its cascade identity.
	outb(PIC1_DATA, 0x01);           // 8086/88 (MCS-80/85) mode.
	outb(PIC2_DATA, 0x01);
	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
}

namespace amd64 {
	namespace irq {
		void bind(int irq, irq_handler_t handler) {
			irq_handlers[irq] = handler;
		}
	}  // namespace irq

	namespace idt {
		// Setup syscall interrupt handler
		static void
		    set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
			Idt_entry[num].base_lo  = (uint16_t) (base & 0xFFFF);
			Idt_entry[num].base_mid = (uint16_t) ((base >> 16) & 0xFFFF);
			Idt_entry[num].base_hi  = (uint32_t) ((base >> 32) & 0xFFFFFFFF);
			Idt_entry[num].sel      = sel;
			Idt_entry[num].ist      = 0;
			Idt_entry[num].flags    = flags;
			Idt_entry[num].reserved = 0;
		}

		void setup_syscall(void) {
			// Register syscall interrupt handler (int 0x80)
			// Use DPL=3 (user-level) to allow user-mode access
			set_gate(idt_constants::SYSCALL_VECTOR,
			         (uint64_t) syscall_int_handler,
			         idt_constants::KERNEL_CODE_SELECTOR,
			         idt_constants::GATE_USER_INTERRUPT);

			logger::debug::printf(
			    "idt",
			    "info",
			    "Syscall interrupt handler registered at 0x80\n");
		}

		void init(void) {
			// Set up the IDT pointer.
			idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
			idtp.base  = (uint64_t) &Idt_entry;

			// Remap the PIC.
			pic_remap(0x20, 0x28);

			// Set up ISR entries using loop
			void *isr_table[] = {
			    (void *) isr0,  (void *) isr1,  (void *) isr2,
			    (void *) isr3,  (void *) isr4,  (void *) isr5,
			    (void *) isr6,  (void *) isr7,  (void *) isr8,
			    (void *) isr9,  (void *) isr10, (void *) isr11,
			    (void *) isr12, (void *) isr13, (void *) isr14,
			    (void *) isr15, (void *) isr16, (void *) isr17,
			    (void *) isr18, (void *) isr19, (void *) isr20,
			    (void *) isr21, (void *) isr22, (void *) isr23,
			    (void *) isr24, (void *) isr25, (void *) isr26,
			    (void *) isr27, (void *) isr28, (void *) isr29,
			    (void *) isr30, (void *) isr31};

			for( int i = 0; i < 32; ++i ) {
				set_gate(static_cast<uint8_t>(i),
				         (uint64_t) isr_table[i],
				         idt_constants::KERNEL_CODE_SELECTOR,
				         idt_constants::GATE_KERNEL_INTERRUPT);
			}

			// Set up IRQ entries using loop
			void *irq_table[] = {(void *) irq0,
			                     (void *) irq1,
			                     (void *) irq2,
			                     (void *) irq3,
			                     (void *) irq4,
			                     (void *) irq5,
			                     (void *) irq6,
			                     (void *) irq7,
			                     (void *) irq8,
			                     (void *) irq9,
			                     (void *) irq10,
			                     (void *) irq11,
			                     (void *) irq12,
			                     (void *) irq13,
			                     (void *) irq14,
			                     (void *) irq15};

			for( int i = 0; i < 16; ++i ) {
				set_gate(
				    static_cast<uint8_t>(idt_constants::IRQ_BASE + i),
				    (uint64_t) irq_table[i],
				    idt_constants::KERNEL_CODE_SELECTOR,
				    idt_constants::GATE_KERNEL_INTERRUPT);
			}

			// Set up syscalls
			setup_syscall();

			// Load the IDT.
			__asm__ volatile("lidt %0" : : "m"(idtp));
		}
	}  // namespace idt
}  // namespace amd64

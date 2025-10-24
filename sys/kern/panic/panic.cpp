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
#include "panic.h"

#include "drv/driver.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/isr/registers.h>
#	include <arch/amd64/cpu/cpuid.h>
#endif
#include <kitoa.h>
#include <kmemset.h>
#include <ksleep.h>
#include <kstdio.h>

#include <kern/acpi/acpi.h>

static int          acpi_action        = 1;  // 0 = poweroff, 1 = reboot
static unsigned int acpi_delay_seconds = 5;

static const char *const panic_messages[] = {
    "Unknown error",                   // PANIC_UNKNOWN_ERROR
    "Division by zero",                // PANIC_DIVISION_BY_ZERO
    "Double fault",                    // PANIC_DOUBLE_FAULT
    "General protection fault",        // PANIC_GENERAL_PROTECTION
    "Page fault",                      // PANIC_PAGE_FAULT
    "Stack segment fault",             // PANIC_STACK_SEGMENT
    "Segment not present",             // PANIC_SEGMENT_NOT_PRESENT
    "Invalid TSS",                     // PANIC_INVALID_TSS
    "Alignment check",                 // PANIC_ALIGNMENT_CHECK
    "Machine check",                   // PANIC_MACHINE_CHECK
    "SIMD exception",                  // PANIC_SIMD_EXCEPTION
    "Virtualization exception",        // PANIC_VIRTUALIZATION
    "Control protection exception",    // PANIC_CONTROL_PROTECTION
    "Hypervisor injection exception",  // PANIC_HYPERVISOR
    "VMM communication exception",     // PANIC_VMM_COMMUNICATION
    "Security exception",              // PANIC_SECURITY
    "Invalid opcode"                   // PANIC_INVALID_OPCODE
};

static bool panic_in_progress = false;

namespace panic {
	static const char *panic_get_message(int code) {
		if( code < 0
		    || code >= (int) (sizeof(panic_messages) / sizeof(*panic_messages)) )
			return "Unknown error";
		return panic_messages[code] ? panic_messages[code] : "Unknown error";
	}

	static void panic_puts(const char *s) {
		serial::writes(s);
		video::puts(s);
	}

	void dump_registers(registers_t *r) {
		char buf[32];

		struct {
			const char *name;
			uint64_t    val;
		} regs[] = {{"RAX", r->rax},
		            {"RBX", r->rbx},
		            {"RCX", r->rcx},
		            {"RDX", r->rdx},
		            {"RSI", r->rsi},
		            {"RDI", r->rdi},
		            {"RBP", r->rbp},
		            {"R8", r->r8},
		            {"R9", r->r9},
		            {"R10", r->r10},
		            {"R11", r->r11},
		            {"R12", r->r12},
		            {"R13", r->r13},
		            {"R14", r->r14},
		            {"R15", r->r15},
		            {"INT_NO", r->int_no},
		            {"ERR_CODE", r->err_code}};

		for( int i = 0; i < (int) (sizeof(regs) / sizeof(*regs)); i++ ) {
			panic_puts(" ");
			panic_puts(regs[i].name);
			panic_puts("=");
			kstd::itoa(buf, buf + 30, (long) regs[i].val, 16, 0);
			panic_puts(buf);
			if( (i + 1) % 4 == 0 )
				panic_puts("\n");
		}
	}

	void init(int code, registers_t *regs) {
		panic_in_progress = true;
		__asm__ volatile("cli");
		video::clear(0xFF0000);

		char        buf[32];
		const char *msg = panic_get_message(code);

		panic_puts("panic(): ");
		kstd::itoa(buf, buf + 30, code, 16, 0);
		panic_puts(buf);
		panic_puts(" (\"");
		panic_puts(msg);
		panic_puts("\")\n");

		if( regs ) {
			panic_puts("\nregister_dump:\n");
			dump_registers(regs);
		}

		panic_puts("cpu: ");
		panic_puts(cpu_brand_string);
		panic_puts("\n");
		panic_puts("cpu_vendor: ");
		panic_puts(cpu_vendor_string);
		kstd::itoa(buf, buf + 30, cpu_core_id, 10, 0);
		panic_puts("\ncpu_core_id: ");
		panic_puts(buf);
		panic_puts("\n");

		for( unsigned int i = acpi_delay_seconds; i > 0; --i ) {
			panic_puts(acpi_action == 0 ? "poweroff: " : "reboot: ");
			kstd::itoa(buf, buf + 30, i, 10, 0);
			panic_puts(buf);
			panic_puts("s\n");
			unistd::sleep::sleep(1000);
		}

		if( acpi_action == 0 )
			acpi::poweroff();
		else
			acpi::reboot();

		for( ;; )
			__asm__ volatile("hlt");
	}
}  // namespace panic

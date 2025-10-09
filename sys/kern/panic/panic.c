// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/panic/panic.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "panic.h"

#include "drv/driver.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/isr/registers.h>
#endif
#include <kitoa.h>
#include <kmemset.h>
#include <ksleep.h>
#include <kstdio.h>

#include <kern/acpi/acpi.h>

// Panic error codes.
#define PANIC_UNKNOWN_ERROR       0
#define PANIC_DIVISION_BY_ZERO    1
#define PANIC_DOUBLE_FAULT        2
#define PANIC_GENERAL_PROTECTION  3
#define PANIC_PAGE_FAULT          4
#define PANIC_STACK_SEGMENT       5
#define PANIC_SEGMENT_NOT_PRESENT 6
#define PANIC_INVALID_TSS         7
#define PANIC_ALIGNMENT_CHECK     8
#define PANIC_MACHINE_CHECK       9
#define PANIC_SIMD_EXCEPTION      10
#define PANIC_VIRTUALIZATION      11
#define PANIC_CONTROL_PROTECTION  12
#define PANIC_HYPERVISOR          13
#define PANIC_VMM_COMMUNICATION   14
#define PANIC_SECURITY            15
#define PANIC_INVALID_OPCODE \
	16  // For some reason, this is every time used instead of the proper ones.

unsigned int seconds_to_reboot = 5;

// Get kpanic message based on error code.
static const char *
    panic_get_message (int code) {
	switch (code) {
		case PANIC_DIVISION_BY_ZERO:
			return "Division by zero";
		case PANIC_DOUBLE_FAULT:
			return "Double fault";
		case PANIC_GENERAL_PROTECTION:
			return "General protection fault";
		case PANIC_PAGE_FAULT:
			return "Page fault";
		case PANIC_STACK_SEGMENT:
			return "Stack segment fault";
		case PANIC_SEGMENT_NOT_PRESENT:
			return "Segment not present";
		case PANIC_INVALID_TSS:
			return "Invalid TSS";
		case PANIC_ALIGNMENT_CHECK:
			return "Alignment check";
		case PANIC_MACHINE_CHECK:
			return "Machine check";
		case PANIC_SIMD_EXCEPTION:
			return "SIMD exception";
		case PANIC_VIRTUALIZATION:
			return "Virtualization exception";
		case PANIC_CONTROL_PROTECTION:
			return "Control protection exception";
		case PANIC_HYPERVISOR:
			return "Hypervisor injection exception";
		case PANIC_VMM_COMMUNICATION:
			return "VMM communication exception";
		case PANIC_SECURITY:
			return "Security exception";
		case PANIC_INVALID_OPCODE:
			return "Invalid opcode";  // I hate you.
		default:
			return "Unknown error";
	}
}

/*
 * Indicates whether we are already inside a kpanic handler. This prevents
 * recursive panics (e.g., if the kpanic handler itself triggers a fault),
 * which otherwise lead to double-/triple-faults and a silent reboot.
 */
static kbool panic_in_progress = kfalse;

/**
 * @brief Output a string in the console and serial
 * @param string Message to output
 */
void
    panic_puts (const char *string) {
	serial.writes(string);
	video.puts(string);
}

/**
 * @brief Dumps the contents of CPU registers to the panic output
 *
 * Prints the values of the general-purpose registers from
 * the provided registers_t structure, which typically holds a snapshot
 * of the processor state during a fault or exception.  The values are
 * printed in hexadecimal format for diagnostics during kernel panic.
 *
 * @param r Pointer to the registers_t structure containing register values
 */
static void
    panic_dump_registers (registers_t *r) {
	char buff[32];

	panic_puts(" RAX=");
	kitoa(buff, buff + 30, (long) r->rax, 16, 0);
	panic_puts(buff);
	panic_puts(" RBX=");
	kitoa(buff, buff + 30, (long) r->rbx, 16, 0);
	panic_puts(buff);
	panic_puts(" RCX=");
	kitoa(buff, buff + 30, (long) r->rcx, 16, 0);
	panic_puts(buff);
	panic_puts(" RDX=");
	kitoa(buff, buff + 30, (long) r->rdx, 16, 0);
	panic_puts(buff);
	panic_puts("\n");

	panic_puts(" RSI=");
	kitoa(buff, buff + 30, (long) r->rsi, 16, 0);
	panic_puts(buff);
	panic_puts(" RDI=");
	kitoa(buff, buff + 30, (long) r->rdi, 16, 0);
	panic_puts(buff);
	panic_puts(" RBP=");
	kitoa(buff, buff + 30, (long) r->rbp, 16, 0);
	panic_puts(buff);

	panic_puts(" R8 =");
	kitoa(buff, buff + 30, (long) r->r8, 16, 0);
	panic_puts(buff);
	panic_puts(" R9 =");
	kitoa(buff, buff + 30, (long) r->r9, 16, 0);
	panic_puts(buff);
	panic_puts(" R10=");
	kitoa(buff, buff + 30, (long) r->r10, 16, 0);
	panic_puts(buff);
	panic_puts(" R11=");
	kitoa(buff, buff + 30, (long) r->r11, 16, 0);
	panic_puts(buff);
	panic_puts("\n");

	panic_puts(" R12=");
	kitoa(buff, buff + 30, (long) r->r12, 16, 0);
	panic_puts(buff);
	panic_puts(" R13=");
	kitoa(buff, buff + 30, (long) r->r13, 16, 0);
	panic_puts(buff);
	panic_puts(" R14=");
	kitoa(buff, buff + 30, (long) r->r14, 16, 0);
	panic_puts(buff);
	panic_puts(" R15=");
	kitoa(buff, buff + 30, (long) r->r15, 16, 0);
	panic_puts(buff);
	panic_puts("\n");
}

/**
 * @brief Handles a kernel panic situation and initiates a system reboot.
 *
 * It disables interrupts, clears the screen, displays an error message (including the error code
 * and its string representation), optionally dumps the CPU registers if provided, and starts a
 * countdown to system reboot.  After displaying the countdown, it triggers the system reboot
 * sequence using ACPI or legacy reboot.
 *
 * @param code The panic error code indicating the cause of the panic
 * @param regs Pointer to a registers_t structure containing register values at the time of the panic,
 *             or NULL if unavailable
 */
void
    panic_init (int code, registers_t *regs) {
	panic_in_progress = ktrue;
	__asm__ volatile("cli");

	video.clear(0x0000ff);

	const char *error_msg = panic_get_message(code);

	panic_puts("\n\nOops! Your system crashed\n");
	panic_puts("Error code: ");
	char buff[16];
	kitoa(buff, buff + 14, code, 10, 0);
	panic_puts(buff);
	panic_puts("\n\nError: ");
	panic_puts(error_msg);
	panic_puts("\n");

	if (regs) {
		panic_puts("\nRegister dump:\n");
		panic_dump_registers(regs);
	}

	panic_puts("System will reboot in ");
	kitoa(buff, buff + 14, seconds_to_reboot, 10, 0);
	panic_puts(buff);
	panic_puts(" seconds...\n");

	//  \_(ツ)_/¯
	for (unsigned int i = seconds_to_reboot; i > 0; --i) {
		panic_puts("Rebooting in ");
		kitoa(buff, buff + 14, i, 10, 0);
		panic_puts(buff);
		panic_puts(" seconds...\n");
		ksleep(1000);
	}

	panic_puts("Rebooting now...\n");
	acpi.reboot();

	for (;;)
		__asm__ volatile("hlt");
}

struct Panic panic = {
    .init    = panic_init,
    .puts    = panic_puts,
    .message = panic_get_message,
    .dump    = panic_dump_registers,
};

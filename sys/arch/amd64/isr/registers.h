#pragma once

#include <kstdint.h>

/**
 * @brief CPU register state pushed onto the stack by ISR/IRQ stubs
 *
 * This structure represents the register state that is automatically pushed
 * onto the stack by the CPU or our ISR/IRQ stubs when an interrupt occurs.
 * A pointer to this struct is passed to the C-level interrupt handlers.
 */
typedef struct {
	kuint64_t r15;       // General-purpose register R15
	kuint64_t r14;       // General-purpose register R14
	kuint64_t r13;       // General-purpose register R13
	kuint64_t r12;       // General-purpose register R12
	kuint64_t r11;       // General-purpose register R11
	kuint64_t r10;       // General-purpose register R10
	kuint64_t r9;        // General-purpose register R9
	kuint64_t r8;        // General-purpose register R8
	kuint64_t rdi;       // General-purpose register RDI
	kuint64_t rsi;       // General-purpose register RSI
	kuint64_t rbp;       // Base pointer register RBP
	kuint64_t rbx;       // General-purpose register RBX
	kuint64_t rdx;       // General-purpose register RDX
	kuint64_t rcx;       // General-purpose register RCX
	kuint64_t rax;       // General-purpose register RAX
	kuint64_t int_no;    // Interrupt number pushed by ISR
	kuint64_t err_code;  // Error code pushed by ISR (if applicable)
} registers_t;

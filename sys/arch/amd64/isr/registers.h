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

#include <kstdint.h>

/**
 * @brief CPU register state pushed onto the stack by ISR/IRQ stubs
 *
 * This structure represents the register state that is automatically pushed
 * onto the stack by the CPU or our ISR/IRQ stubs when an interrupt occurs.
 * A pointer to this struct is passed to the C-level interrupt handlers.
 */
typedef struct {
	uint64_t r15;       // General-purpose register R15
	uint64_t r14;       // General-purpose register R14
	uint64_t r13;       // General-purpose register R13
	uint64_t r12;       // General-purpose register R12
	uint64_t r11;       // General-purpose register R11
	uint64_t r10;       // General-purpose register R10
	uint64_t r9;        // General-purpose register R9
	uint64_t r8;        // General-purpose register R8
	uint64_t rdi;       // General-purpose register RDI
	uint64_t rsi;       // General-purpose register RSI
	uint64_t rbp;       // Base pointer register RBP
	uint64_t rbx;       // General-purpose register RBX
	uint64_t rdx;       // General-purpose register RDX
	uint64_t rcx;       // General-purpose register RCX
	uint64_t rax;       // General-purpose register RAX
	uint64_t int_no;    // Interrupt number pushed by ISR
	uint64_t err_code;  // Error code pushed by ISR (if applicable)
} registers_t;

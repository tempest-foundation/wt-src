// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/drivers/keyboard/keyboard.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <arch/amd64/idt.h>

extern struct Keyboard keyboard;

int
    getchar (void);

struct Keyboard {
	void (*init)(void);
	void (*handle)(registers_t *regs);
	unsigned char (*adjust_case)(unsigned char c);
};

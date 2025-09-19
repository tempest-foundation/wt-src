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

#pragma once

#ifdef ARCH_AMD64
#	include <arch/amd64/idt/idt.h>
#endif

extern struct Keyboard keyboard;

int
    getchar (void);

struct Keyboard {
	void (*init)(void);
	void (*handle)(registers_t *regs);
	unsigned char (*adjust_case)(unsigned char c);
};

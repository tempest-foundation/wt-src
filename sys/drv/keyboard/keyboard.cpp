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
#include <drv/keyboard/keyboard.hpp>
#include <drv/tty/tty.hpp>

#ifdef ARCH_AMD64
#	include <arch/amd64/asm/io.hpp>
#	include <arch/amd64/idt/idt.hpp>
#endif
#include <kstdio.hpp>

#define KBD_DATA_PORT   0x60
#define KBD_BUFFER_SIZE 256

// Simple keyboard buffer
static unsigned char kbd_buffer[KBD_BUFFER_SIZE];
static int           kbd_buffer_head = 0;
static int           kbd_buffer_tail = 0;
static bool          shift_pressed   = false;
static bool          caps_lock       = false;

// Scancode set 1 to ASCII mapping for US keyboard layout
static unsigned char kbd_us[128] = {
    0,   27,   '1',  '2', '3',  '4', '5', '6', '7', '8', '9', '0', '-',
    '=', '\b', '\t', 'q', 'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']',  '\n', 0,   'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,    0,   0,   0,   '-', 0,   0,   0,   /* + -> */ 0,
    0,   0,    0,    0,   0,    0,   0,   0,   0,   0,
};

static unsigned char kbd_us_shift[128] = {
    0,   27,   '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
    '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '"',  '~',  0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
    '>', '?',  0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,   0,   0,   0,   '-', 0,   0,   0,   /* + -> */ 0,
    0,   0,    0,    0,   0,   0,   0,   0,   0,   0,
};

namespace keyboard {
	static unsigned char adjust_case(unsigned char c) {
		if( caps_lock && !shift_pressed && c >= 'a' && c <= 'z' )
			return (unsigned char) (c - 32);
		if( caps_lock && shift_pressed && c >= 'A' && c <= 'Z' )
			return (unsigned char) (c + 32);
		return c;
	}

	static void handler(registers_t *regs) {
		(void) regs;
		unsigned char scancode = inb(KBD_DATA_PORT);

		// Caps Lock pressed
		if( scancode == 0x3A ) {
			caps_lock = !caps_lock;  // Toggle caps lock
			return;
		}

		if( scancode == 0x2A || scancode == 0x36 ) {
			shift_pressed = true;
			return;
		}
		if( scancode == 0xAA || scancode == 0xB6 ) {
			shift_pressed = false;
			return;
		}

		if( scancode < 128 ) {
			unsigned char c = shift_pressed ? kbd_us_shift[scancode]
			                                : kbd_us[scancode];

			c = adjust_case(c);

			if( c != 0 ) {
				tty::receive_char((char) c);
			}
		}
	}

	/*
	 ! This only works in single-tasking environments.
	 ! Needs locking or buffer protection for multi-tasking.
	 */
	int getchar(void) {
		// Wait for a character to be available
		while( kbd_buffer_head == kbd_buffer_tail ) {
			__asm__ volatile("sti; hlt");
		}
		__asm__ volatile("cli");

		int c           = kbd_buffer[kbd_buffer_tail];
		kbd_buffer_tail = (kbd_buffer_tail + 1) % KBD_BUFFER_SIZE;

		__asm__ volatile("sti");
		return c;
	}

	void init(void) {
		amd64::irq::bind(1, handler);
	}
}  // namespace keyboard

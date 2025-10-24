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
#include "tty.h"

#include <drv/keyboard/keyboard.h>
#include <drv/video/video.h>

struct Main_tty main_tty;

namespace tty {
	void receive_char(char c) {
		int next = (main_tty.head + 1) % TTY_BUF_SIZE;
		if( next != main_tty.tail ) {
			main_tty.input_buf[main_tty.head] = c;
			main_tty.head                     = next;

			// ONLY echo safe characters - NEVER echo control chars
			if( main_tty.echo ) {
				unsigned char uc = (unsigned char) c;

				if( c == '\b' || c == 127 ) {
					main_tty.write_char('\b');
					main_tty.write_char(' ');
					main_tty.write_char('\b');
				} else if( uc >= 32 && uc <= 126 ) {
					main_tty.write_char(c);
				}
				// Everything else: NO ECHO (includes \r, \n, \t, ESC, etc)
			}
		}
	}

	int read_char(void) {
		while( main_tty.head == main_tty.tail )
			__asm__ volatile("sti; hlt");

		__asm__ volatile("cli");
		int c         = main_tty.input_buf[main_tty.tail];
		main_tty.tail = (main_tty.tail + 1) % TTY_BUF_SIZE;
		__asm__ volatile("sti");

		// Convert CR to LF
		if( c == '\r' )
			c = '\n';

		return c;
	}

	void write(const char *s) {
		while( *s )
			main_tty.write_char(*s++);
	}

	void init(void) {
		main_tty.head       = 0;
		main_tty.tail       = 0;
		main_tty.echo       = 1;
		main_tty.write_char = video::putchar;

		keyboard::init();
		video::clear(0x000000);
	}
}  // namespace tty

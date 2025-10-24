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
#include "serial.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/asm/io.h>
#endif
#include <kitoa.h>

#define PORT 0x3f8  // COM1

namespace serial {
	static int serial_is_transmit_empty(void) {
		return inb(PORT + 5) & 0x20;
	}

	bool is_available(void) {
		// Save original values.
		unsigned char original_lcr = inb(PORT + 3);
		unsigned char original_mcr = inb(PORT + 4);

		// Set up loopback mode.
		outb(PORT + 3, 0x80);  // Enable DLAB.
		outb(PORT + 4, 0x10);  // Enable loopback mode.

		// Test value to write and read back.
		unsigned char test_value = 0xAA;

		// Write test value.
		outb(PORT, test_value);

		// Small delay to ensure data is processed.
		for( int i = 0; i < 1000; i++ )
			;

		// Read back the value.
		unsigned char read_value = inb(PORT);

		// Restore original values.
		outb(PORT + 3, original_lcr);
		outb(PORT + 4, original_mcr);

		// If read value matches test value, port is available.
		return (read_value == test_value);
	}

	void init(void) {
		if( !is_available() )
			return;        // Exit if serial not available.
		outb(PORT + 1, 0x00);  // Disable all interrupts.
		outb(PORT + 3, 0x80);  // Enable DLAB (set baud rate divisor).
		outb(PORT + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
		outb(PORT + 1, 0x00);  //                  (hi byte)
		outb(PORT + 3, 0x03);  // 8 bits, no parity, one stop bit.
		outb(PORT + 2,
		     0xC7);            // Enable FIFO, clear them, with 14-byte threshold.
		outb(PORT + 4, 0x0B);  // IRQs enabled, RTS/DSR set.
	}

	void write(char a) {
		if( !is_available() || !a )
			return;  // Exit if serial not available or if char is null.
		while( serial_is_transmit_empty() == 0 )
			;
		outb(PORT, (unsigned char) a);
	}

	void writes(const char *s) {
		if( !is_available() || !s )
			return;  // Exit if serial not available or if string is null.
		while( *s ) {
			write(*s++);
		}
	}

	void write_int(int i) {
		if( !is_available() )
			return;  // Exit if serial not available.
		char buf[16];
		kstd::itoa(buf, buf + 15, i, 10, 0);
		writes(buf);
	}
}  // namespace serial

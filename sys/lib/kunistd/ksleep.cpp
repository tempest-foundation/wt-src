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
#ifdef ARCH_AMD64
#	include <arch/amd64/asm/io.h>
#endif
#include <ksleep.h>
#include <kstddef.h>
#include <kstdint.h>

volatile uint64_t *hpet = (volatile uint64_t *) 0xFED00000;

#define HPET_GEN_CONF        0x10
#define HPET_MAIN_COUNTER    0xF0
#define HPET_CAP_ID          0x0
#define HPET_GEN_CONF_ENABLE (1 << 0)

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

namespace unistd {
	namespace sleep {
		/**
 * @brief Enables the HPET timer if it is not already enabled
 */
		static void hpet_enable(void) {
			if( (hpet[HPET_GEN_CONF / 8] & HPET_GEN_CONF_ENABLE) == 0 ) {
				hpet[HPET_GEN_CONF / 8] |= HPET_GEN_CONF_ENABLE;
			}
		}

		/**
 * @brief Busy-wait for an approximate duration in milliseconds using the PIT
 *
 * @param ms Number of milliseconds to wait
 */
		static void pit_wait(int ms) {
			outb(PIT_COMMAND, 0x34);

			uint16_t reload = 1193;  // ~1 ms (1193182 Hz / 1000)
			outb(PIT_CHANNEL0, (uint8_t) (reload & 0xFF));
			outb(PIT_CHANNEL0, (uint8_t) ((reload >> 8) & 0xFF));

			for( int i = 0; i < ms; i++ ) {
				uint8_t prev = 0xFF, curr = 0;
				do {
					curr = inb(PIT_CHANNEL0);
					if( curr > prev )
						break;
					prev = curr;
				} while( curr != 0 );
			}
		}

		/**
 * @brief Sleeps for a given number of milliseconds
 *
 * Uses HPET for high-resolution sleep if available, otherwise falls back to PIT busy-wait.
 *
 * @param ms Number of milliseconds to sleep
 */
		void sleep(int ms) {
			if( hpet != nullptr ) {
				hpet_enable();

				uint64_t period_fs = hpet[HPET_CAP_ID / 8] >> 32;
				uint64_t start     = hpet[HPET_MAIN_COUNTER / 8];

				uint64_t ticks =
				    (uint64_t) ms * 1000000000ULL / (period_fs / 1000);

				while( (hpet[HPET_MAIN_COUNTER / 8] - start) < ticks ) {
					cpu_relax();
				}
			} else {
				pit_wait(ms);
			}
		}
	}  // namespace sleep
}  // namespace unistd

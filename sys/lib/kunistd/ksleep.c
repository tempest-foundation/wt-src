#ifdef ARCH_AMD64
#	include <arch/amd64/asm/kio.h>
#endif
#include <ksleep.h>
#include <kstddef.h>
#include <kstdint.h>

volatile kuint64_t *hpet = (volatile kuint64_t *) 0xFED00000;

#define HPET_GEN_CONF        0x10
#define HPET_MAIN_COUNTER    0xF0
#define HPET_CAP_ID          0x0
#define HPET_GEN_CONF_ENABLE (1 << 0)

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

/**
 * @brief Enables the HPET timer if it is not already enabled
 */
void
    khpet_enable (void) {
	if ((hpet[HPET_GEN_CONF / 8] & HPET_GEN_CONF_ENABLE) == 0) {
		hpet[HPET_GEN_CONF / 8] |= HPET_GEN_CONF_ENABLE;
	}
}

/**
 * @brief Busy-wait for an approximate duration in milliseconds using the PIT
 *
 * @param ms Number of milliseconds to wait
 */
void
    kpit_wait (int ms) {
	koutb(PIT_COMMAND, 0x34);

	kuint16_t reload = 1193;  // ~1 ms (1193182 Hz / 1000)
	koutb(PIT_CHANNEL0, (kuint8_t) (reload & 0xFF));
	koutb(PIT_CHANNEL0, (kuint8_t) ((reload >> 8) & 0xFF));

	for (int i = 0; i < ms; i++) {
		kuint8_t prev = 0xFF, curr = 0;
		do {
			curr = kinb(PIT_CHANNEL0);
			if (curr > prev)
				break;
			prev = curr;
		} while (curr != 0);
	}
}

/**
 * @brief Sleeps for a given number of milliseconds
 *
 * Uses HPET for high-resolution sleep if available, otherwise falls back to PIT busy-wait.
 *
 * @param ms Number of milliseconds to sleep
 */
void
    ksleep (int ms) {
	if (hpet != KNULL) {
		khpet_enable();

		kuint64_t period_fs = hpet[HPET_CAP_ID / 8] >> 32;
		kuint64_t start     = hpet[HPET_MAIN_COUNTER / 8];

		kuint64_t ticks = (kuint64_t) ms * 1000000000ULL / (period_fs / 1000);

		while ((hpet[HPET_MAIN_COUNTER / 8] - start) < ticks) {
			kcpu_relax();
		}
	} else {
		kpit_wait(ms);
	}
}

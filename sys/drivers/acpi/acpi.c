#include "acpi.h"

#include <arch/amd64/kasm/kio.h>
#include <debug/debug.h>
#include <lib/kstdio/kstddef.h>
#include <lib/kunistd/ksleep.h>

/*
 * This is not ACPI yet, this is a placeholder for ACPI.
 * Since I will polish the Tempest kernel before I decide
 * to lose my precious time on ACPI.
 */

// ? Maybe use uACPI?

// Define timeout for poweroff in milliseconds
#define POWEROFF_TIMEOUT_MS 5000

void
    a_poweroff (void) {
	koutw(0x604, 0x2000);   // Port 0x604
	koutw(0xB004, 0x2000);  // Port 0xB004

	ksleep(POWEROFF_TIMEOUT_MS);
	// If poweroff fails, log warning
	debug.err("acpi", "Legacy poweroff failed", KNULL);
}

void
    a_reboot (void) {
	while (kinb(0x64) & 0x02)
		;           // Wait for keyboard controller ready
	koutb(0x64, 0xFE);  // Send reset command

	ksleep(POWEROFF_TIMEOUT_MS);
	// If reboot fails, log warning
	debug.err("acpi", "Legacy reboot failed", KNULL);
}

struct Acpi acpi = {.poweroff = a_poweroff, .reboot = a_reboot};

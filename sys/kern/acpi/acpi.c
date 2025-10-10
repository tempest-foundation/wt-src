#include "acpi.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/asm/kio.h>
#endif
#include <ksleep.h>
#include <kstddef.h>

#include <dbg/logger.h>

/*
 * This is not ACPI yet, this is a placeholder for ACPI.
 * Since I will polish the Tempest kernel before I decide
 * to lose my precious time on ACPI.
 */

// ? Maybe use uACPI?

// Define timeout for poweroff in milliseconds
#define POWEROFF_TIMEOUT_MS 5000

/**
 * @brief Legacy poweroff function that uses the legacy BIOS ports
 * @warning If the ports fail, the machine may not power off because of modern hardware
 * making this method deprecated
 */
void
    acpi_poweroff (void) {
	koutw(0x604, 0x2000);   // Port 0x604
	koutw(0xB004, 0x2000);  // Port 0xB004

	ksleep(POWEROFF_TIMEOUT_MS);
	// If poweroff fails, log warning
	logger.err("acpi", "Legacy poweroff failed", KNULL);
}

/**
 * @brief Legacy reboot function that uses the keyboard controller
 * @warning If the ports fail, the machine may not power off because of modern hardware
 * making this method deprecated
 */
void
    acpi_reboot (void) {
	while (kinb(0x64) & 0x02)
		;           // Wait for keyboard controller ready
	koutb(0x64, 0xFE);  // Send reset command

	ksleep(POWEROFF_TIMEOUT_MS);
	// If reboot fails, log warning
	logger.err("acpi", "Legacy reboot failed", KNULL);
}

struct Acpi acpi = {.poweroff = acpi_poweroff, .reboot = acpi_reboot};

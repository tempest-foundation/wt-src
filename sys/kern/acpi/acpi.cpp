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
#include "acpi.h"

#ifdef ARCH_AMD64
#	include <arch/amd64/asm/io.h>
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

namespace acpi {
	/**
	 * @brief Legacy poweroff function that uses the legacy BIOS ports
	 * @warning If the ports fail, the machine may not power off because of modern hardware
	 * making this method deprecated
	 */
	void poweroff(void) {
		outw(0x604, 0x2000);   // Port 0x604
		outw(0xB004, 0x2000);  // Port 0xB004

		unistd::sleep::sleep(POWEROFF_TIMEOUT_MS);
		// If poweroff fails, log warning
		logger::error("acpi", "Legacy poweroff failed", nullptr);
	}

	/**
	 * @brief Legacy reboot function that uses the keyboard controller
	 * @warning If the ports fail, the machine may not power off because of modern hardware
	 * making this method deprecated
	 */
	void reboot(void) {
		while( inb(0x64) & 0x02 )
			;          // Wait for keyboard controller ready
		outb(0x64, 0xFE);  // Send reset command

		unistd::sleep::sleep(POWEROFF_TIMEOUT_MS);
		// If reboot fails, log warning
		logger::error("acpi", "Legacy reboot failed", nullptr);
	}
}  // namespace acpi

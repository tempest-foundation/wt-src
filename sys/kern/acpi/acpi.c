// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/acpi/acpi.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "acpi.h"

#include <arch/amd64/asm/kio.h>
#include <dbg/logger.h>
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
    acpi_poweroff (void) {
	koutw(0x604, 0x2000);   // Port 0x604
	koutw(0xB004, 0x2000);  // Port 0xB004

	ksleep(POWEROFF_TIMEOUT_MS);
	// If poweroff fails, log warning
	logger.err("acpi", "Legacy poweroff failed", KNULL);
}

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

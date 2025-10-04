// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/kfmod.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kfmod.h>
#include <kmath.h>

double
    kfmod (double x, double y) {
	if (y == 0.0) {
		// Return NaN for division by zero
		return 0.0;
	}

	if (x == 0.0) {
		return 0.0;
	}

	// Calculate remainder using: x - y * floor(x/y)
	double quotient = x / y;
	double int_part = kfloor(quotient);
	return x - y * int_part;
}

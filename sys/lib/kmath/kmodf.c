// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/kmodf.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmath.h>
#include <kmodf.h>

double
    kmodf (double x, double *iptr) {
	// Handle special cases
	if (kisnan(x)) {
		*iptr = knan("");
		return knan("");
	}
	if (kisinf(x)) {
		*iptr = x;
		return 0.0;
	}

	// Extract integer part
	if (x >= 0.0)
		*iptr = kfloor(x);
	else
		*iptr = kceil(x);

	// Return fractional part
	return x - *iptr;
}

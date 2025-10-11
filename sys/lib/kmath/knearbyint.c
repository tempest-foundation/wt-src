// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/knearbyint.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmath.h>
#include <knearbyint.h>

double
    knearbyint (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (kisinf(x))
		return x;

	// Round to nearest integer (same as rint for this implementation)
	if (x >= 0.0) {
		double frac = x - kfloor(x);
		if (frac < 0.5)
			return kfloor(x);
		else
			return kceil(x);
	} else {
		double frac = kceil(x) - x;
		if (frac < 0.5)
			return kceil(x);
		else
			return kfloor(x);
	}
}

// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/khypot.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <lib/kmath/khypot.h>
#include <lib/kmath/kmath.h>

double
    khypot (double x, double y) {
	// Handle special cases
	if (kisinf(x) || kisinf(y)) {
		return 1.0 / 0.0;  // Return infinity
	}

	if (kisnan(x) || kisnan(y)) {
		return 0.0;  // Return NaN
	}

	// Use the formula: ksqrt(x^2 + y^2)
	return ksqrt(x * x + y * y);
}

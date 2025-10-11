// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/ksqrt.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmath.h>
#include <ksqrt.h>

double
    ksqrt (double x) {
	if (x < 0.0) {
		// Return NaN for negative numbers
		return 0.0;
	}

	if (x == 0.0 || x == 1.0) {
		return x;
	}

	// Newton-Raphson method for square root
	double guess = x / 2.0;
	double prev_guess;

	do {
		prev_guess = guess;
		guess      = (guess + x / guess) / 2.0;
	} while (kfabs(guess - prev_guess) > 1e-15);

	return guess;
}

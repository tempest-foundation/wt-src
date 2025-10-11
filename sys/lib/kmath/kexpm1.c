// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/kexpm1.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kexpm1.h>
#include <kmath.h>

double
    kexpm1 (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (kisinf(x))
		return x > 0 ? K_INFINITY : -1.0;

	// For small values, use Taylor series to avoid loss of precision
	if (kfabs(x) < 0.1) {
		double x2 = x * x;
		double x3 = x2 * x;
		double x4 = x3 * x;
		double x5 = x4 * x;
		return x + x2 / 2.0 + x3 / 6.0 + x4 / 24.0 + x5 / 120.0;
	}

	// For larger values, use exp(x) - 1
	return kexp(x) - 1.0;
}

// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/kacos.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kacos.h>
#include <kmath.h>

double
    kacos (double x) {
	// Check for invalid input
	if (x < -1.0 || x > 1.0)
		return knan("");

	// Use the relationship: acos(x) = π/2 - asin(x)
	return K_M_PI_2 - kasin(x);
}

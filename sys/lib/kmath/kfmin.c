// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/kfmin.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <lib/kmath/kfmin.h>
#include <lib/kmath/kmath.h>

double
    kfmin (double x, double y) {
	// Handle NaN cases
	if (x != x)
		return y;
	if (y != y)
		return x;

	return x < y ? x : y;
}

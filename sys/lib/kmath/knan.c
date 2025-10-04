// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/knan.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmath.h>
#include <knan.h>

double
    knan (const char *tagp) {
	// Return NaN - the tagp parameter is ignored in this implementation
	(void) tagp;  // Suppress unused parameter warning

	// Return a quiet NaN
	union {
		double             d;
		unsigned long long u;
	} u;

	u.u = 0x7ff8000000000000ULL;  // Quiet NaN
	return u.d;
}

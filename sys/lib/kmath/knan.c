// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/lib/kmath/knan.c
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <lib/kmath/kmath.h>
#include <lib/kmath/knan.h>

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

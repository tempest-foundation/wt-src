// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#include <kmath.h>
#include <knan.h>

namespace kmath {
	double nan(const char *tagp) {
		// Return NaN --- the tagp parameter is ignored in this implementation
		(void) tagp;  // Suppress unused parameter warning

		// Return a quiet NaN
		union {
			double             d;
			unsigned long long u;
		} u;

		u.u = 0x7ff8000000000000ULL;  // Quiet NaN
		return u.d;
	}
}  // namespace kmath

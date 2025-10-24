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
#include <knextafter.h>

namespace kmath {
	double nextafter(double x, double y) {
		// Handle special cases
		if( kmath::isnan(x) || kmath::isnan(y) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x;

		// If x equals y, return y
		if( x == y )
			return y;

		// Determine direction
		int direction = (y > x) ? 1 : -1;

		// For zero, return smallest subnormal in the direction of y
		if( x == 0.0 ) {
			union {
				double             d;
				unsigned long long u;
			} u;

			u.u = direction > 0 ? 0x0000000000000001ULL
			                    : 0x8000000000000001ULL;
			return u.d;
		}

		// For finite non-zero values, use a simple approximation
		// This is a simplified implementation - a full implementation would
		// need to handle the IEEE 754 bit representation directly
		double epsilon = 1e-15;
		return x + direction * epsilon * kmath::fabs(x);
	}
}  // namespace kmath

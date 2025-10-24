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
#include <kremainder.h>

namespace kmath {
	double remainder(double x, double y) {
		// Handle special cases
		if( kmath::isnan(x) || kmath::isnan(y) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return kmath::nan("");
		if( y == 0.0 )
			return kmath::nan("");
		if( kmath::isinf(y) )
			return x;

		// Calculate remainder using fmod and adjust for rounding
		double r = kmath::fmod(x, y);

		// Adjust to get remainder in the range [-|y|/2, |y|/2]
		if( kmath::fabs(r) > kmath::fabs(y) / 2.0 ) {
			if( r > 0 )
				r -= kmath::fabs(y);
			else
				r += kmath::fabs(y);
		}

		return r;
	}
}  // namespace kmath

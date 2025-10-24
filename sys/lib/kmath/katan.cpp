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
#include <katan.h>
#include <kmath.h>

namespace kmath {
	double atan(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x > 0 ? K_M_PI_2 : -K_M_PI_2;
		if( x == 0.0 )
			return 0.0;

		// Use series expansion for small values
		if( kmath::fabs(x) < 1.0 ) {
			double x2 = x * x;
			double x3 = x2 * x;
			double x5 = x3 * x2;
			double x7 = x5 * x2;
			double x9 = x7 * x2;
			return x - x3 / 3.0 + x5 / 5.0 - x7 / 7.0 + x9 / 9.0;
		}

		// For large values, use atan(x) = π/2 - atan(1/x)
		if( x > 0 )
			return K_M_PI_2 - kmath::atan(1.0 / x);
		else
			return -K_M_PI_2 - kmath::atan(1.0 / x);
	}
}  // namespace kmath

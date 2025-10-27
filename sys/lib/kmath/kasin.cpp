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
#include <kasin.hpp>
#include <kmath.hpp>

namespace kmath {
	double asin(double x) {
		// Check for invalid input
		if( x < -1.0 || x > 1.0 )
			return kmath::nan("");

		// Use Newton's method to find arcsin
		if( kmath::fabs(x) < 0.5 ) {
			double y  = x;
			double y2 = y * y;
			double y3 = y2 * y;
			double y5 = y3 * y2;
			double y7 = y5 * y2;
			return y + y3 / 6.0 + 3.0 * y5 / 40.0 + 5.0 * y7 / 112.0;
		}

		// For values closer to ±1, use different approach
		if( x > 0 )
			return K_M_PI_2 - kmath::asin(kmath::sqrt(1.0 - x * x));
		else
			return -K_M_PI_2 + kmath::asin(kmath::sqrt(1.0 - x * x));
	}
}  // namespace kmath

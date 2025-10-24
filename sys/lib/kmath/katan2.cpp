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
#include <katan2.h>
#include <kmath.h>

namespace kmath {
	double atan2(double y, double x) {
		// Handle special cases
		if( kmath::isnan(x) || kmath::isnan(y) )
			return kmath::nan("");
		if( kmath::isinf(x) && kmath::isinf(y) )
			return kmath::nan("");

		// If x is zero
		if( x == 0.0 ) {
			if( y > 0.0 )
				return K_M_PI_2;
			if( y < 0.0 )
				return -K_M_PI_2;
			return 0.0;  // y is also zero
		}

		// If y is zero
		if( y == 0.0 ) {
			if( x > 0.0 )
				return 0.0;
			if( x < 0.0 )
				return K_M_PI;
			return 0.0;
		}

		// If x is infinite
		if( kmath::isinf(x) ) {
			if( x > 0.0 )
				return y > 0.0 ? 0.0 : -0.0;
			else
				return y > 0.0 ? K_M_PI : -K_M_PI;
		}

		// If y is infinite
		if( kmath::isinf(y) ) {
			return y > 0.0 ? K_M_PI_2 : -K_M_PI_2;
		}

		// Normal case: use atan(y/x) with proper quadrant adjustment
		double result = kmath::atan(y / x);

		if( x < 0.0 ) {
			if( y >= 0.0 )
				result += K_M_PI;
			else
				result -= K_M_PI;
		}

		return result;
	}
}  // namespace kmath

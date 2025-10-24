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
#include <kcbrt.h>
#include <kmath.h>

namespace kmath {
	double cbrt(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x;
		if( x == 0.0 )
			return 0.0;

		// Use Newton's method for cube root
		double y = kmath::fabs(x);
		double guess;

		// Initial guess
		if( y > 1.0 )
			guess = y / 3.0;
		else
			guess = y * 2.0 / 3.0;

		// Newton iteration: x_{n+1} = (2*x_n + y/x_n^2) / 3
		for( int i = 0; i < 10; i++ ) {
			double guess2    = guess * guess;
			double new_guess = (2.0 * guess + y / guess2) / 3.0;
			if( fabs(new_guess - guess) < 1e-15 )
				break;
			guess = new_guess;
		}

		return x < 0.0 ? -guess : guess;
	}
}  // namespace kmath

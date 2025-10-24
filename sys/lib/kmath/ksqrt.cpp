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
#include <ksqrt.h>

namespace kmath {
	double sqrt(double x) {
		if( x < 0.0 ) {
			// Return NaN for negative numbers
			return 0.0;
		}

		if( x == 0.0 || x == 1.0 ) {
			return x;
		}

		// Newton-Raphson method for square root
		double guess = x / 2.0;
		double prev_guess;

		do {
			prev_guess = guess;
			guess      = (guess + x / guess) / 2.0;
		} while( kmath::fabs(guess - prev_guess) > 1e-15 );

		return guess;
	}
}  // namespace kmath

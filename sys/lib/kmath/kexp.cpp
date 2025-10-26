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
#include <kexp.h>
#include <kmath.h>

namespace kmath {
	double exp(double x) {
		// Handle special cases
		if( x == 0.0 )
			return 1.0;
		if( x < -700.0 )
			return 0.0;  // Underflow
		if( x > 700.0 )
			return 1.0 / 0.0;  // Overflow

		// Use Taylor series: e^x = 1 + x + x^2/2! + x^3/3! + ...
		double result    = 1.0;
		double term      = 1.0;
		double factorial = 1.0;

		for( int i = 1; i <= 20; i++ ) {
			term *= x;
			factorial *= i;
			result += term / factorial;
		}

		return result;
	}
}  // namespace kmath

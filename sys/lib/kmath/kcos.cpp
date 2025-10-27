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
#include <kcos.hpp>
#include <kmath.hpp>

namespace kmath {
	double cos(double x) {
		// Normalize x to [-π, π]
		x = kmath::fmod(x, 2.0 * K_M_PI);
		if( x > K_M_PI )
			x -= 2.0 * K_M_PI;
		if( x < -K_M_PI )
			x += 2.0 * K_M_PI;

		// Use Taylor series for small values
		if( kmath::fabs(x) < 0.1 ) {
			double x2 = x * x;
			double x4 = x2 * x2;
			double x6 = x4 * x2;
			return 1.0 - x2 / 2.0 + x4 / 24.0 - x6 / 720.0;
		}

		// For larger values, use more terms of Taylor series
		double result    = 0.0;
		double term      = 1.0;
		double x2        = x * x;
		double factorial = 1.0;
		int    sign      = 1;

		for( int i = 0; i <= 10; i++ ) {
			result += sign * term / factorial;
			term *= x2;
			factorial *= (2 * i + 1) * (2 * i + 2);
			sign = -sign;
		}

		return result;
	}
}  // namespace kmath

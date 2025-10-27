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
#include <kfmod.hpp>
#include <kmath.hpp>

namespace kmath {
	double fmod(double x, double y) {
		if( y == 0.0 ) {
			// Return NaN for division by zero
			return 0.0;
		}

		if( x == 0.0 ) {
			return 0.0;
		}

		// Calculate remainder using: x - y * floor(x/y)
		double quotient = x / y;
		double int_part = kmath::floor(quotient);
		return x - y * int_part;
	}
}  // namespace kmath

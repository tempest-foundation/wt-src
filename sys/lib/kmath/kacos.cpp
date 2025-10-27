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
#include <kacos.hpp>
#include <kmath.hpp>

namespace kmath {
	double acos(double x) {
		// Check for invalid input
		if( x < -1.0 || x > 1.0 )
			return kmath::nan("");

		// Use the relationship: acos(x) = π/2 - asin(x)
		return K_M_PI_2 - kmath::asin(x);
	}
}  // namespace kmath

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
#include <khypot.hpp>
#include <kmath.hpp>

namespace kmath {
	double hypot(double x, double y) {
		// Handle special cases
		if( kmath::isinf(x) || kmath::isinf(y) ) {
			return 1.0 / 0.0;  // Return infinity
		}

		if( kmath::isnan(x) || kmath::isnan(y) ) {
			return 0.0;  // Return NaN
		}

		// Use the formula: sqrt(x^2 + y^2)
		return kmath::sqrt(x * x + y * y);
	}
}  // namespace kmath

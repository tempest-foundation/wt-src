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
#include <kfrexp.hpp>
#include <kmath.hpp>

namespace kmath {
	double frexp(double x, int *exp) {
		// Handle special cases
		if( kmath::isnan(x) ) {
			*exp = 0;
			return kmath::nan("");
		}
		if( kmath::isinf(x) ) {
			*exp = 0;
			return x;
		}
		if( x == 0.0 ) {
			*exp = 0;
			return 0.0;
		}

		// Extract exponent using log2
		*exp = (int) kmath::floor(kmath::log2(kmath::fabs(x))) + 1;

		// Calculate mantissa: x / 2^exp
		return x / kmath::pow(2.0, *exp);
	}
}  // namespace kmath

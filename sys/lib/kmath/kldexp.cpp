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
#include <kldexp.hpp>
#include <kmath.hpp>

namespace kmath {
	double ldexp(double x, int exp) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x;
		if( x == 0.0 )
			return 0.0;

		// Use the relationship: ldexp(x, exp) = x * 2^exp
		return x * kmath::pow(2.0, exp);
	}
}  // namespace kmath

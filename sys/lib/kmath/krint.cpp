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
#include <krint.h>

namespace kmath {
	double rint(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x;

		// Round to nearest integer
		if( x >= 0.0 ) {
			double frac = x - kmath::floor(x);
			if( frac < 0.5 )
				return kmath::floor(x);
			else
				return kmath::ceil(x);
		} else {
			double frac = kmath::ceil(x) - x;
			if( frac < 0.5 )
				return kmath::ceil(x);
			else
				return kmath::floor(x);
		}
	}
}  // namespace kmath

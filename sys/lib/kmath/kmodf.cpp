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
#include <kmodf.h>

namespace kmath {
	double modf(double x, double *iptr) {
		// Handle special cases
		if( kmath::isnan(x) ) {
			*iptr = kmath::nan("");
			return kmath::nan("");
		}
		if( kmath::isinf(x) ) {
			*iptr = x;
			return 0.0;
		}

		// Extract integer part
		if( x >= 0.0 )
			*iptr = kmath::floor(x);
		else
			*iptr = kmath::ceil(x);

		// Return fractional part
		return x - *iptr;
	}
}  // namespace kmath

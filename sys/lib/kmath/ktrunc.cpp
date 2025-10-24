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
#include <ktrunc.h>

namespace kmath {
	double trunc(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x;

		// For positive numbers, truncate towards zero
		if( x >= 0.0 )
			return kmath::floor(x);
		else
			return kmath::ceil(x);
	}
}  // namespace kmath

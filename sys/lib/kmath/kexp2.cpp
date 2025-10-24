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
#include <kexp2.h>
#include <kmath.h>

namespace kmath {
	double exp2(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( kmath::isinf(x) )
			return x > 0 ? K_INFINITY : 0.0;

		// Use the relationship: exp2(x) = exp(x * ln(2))
		return kmath::exp(x * K_M_LN2);
	}
}  // namespace kmath

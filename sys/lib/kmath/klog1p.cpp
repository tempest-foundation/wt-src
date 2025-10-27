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
#include <klog1p.hpp>
#include <kmath.hpp>

namespace kmath {
	double log1p(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( x < -1.0 )
			return kmath::nan("");
		if( x == -1.0 )
			return -K_INFINITY;
		if( kmath::isinf(x) )
			return x > 0 ? K_INFINITY : kmath::nan("");

		// For small values, use Taylor series to avoid loss of precision
		if( kmath::fabs(x) < 0.1 ) {
			double x2 = x * x;
			double x3 = x2 * x;
			double x4 = x3 * x;
			double x5 = x4 * x;
			return x - x2 / 2.0 + x3 / 3.0 - x4 / 4.0 + x5 / 5.0;
		}

		// For larger values, use log(1 + x)
		return kmath::log(1.0 + x);
	}
}  // namespace kmath

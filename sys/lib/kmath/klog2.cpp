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
#include <klog2.h>
#include <kmath.h>

namespace kmath {
	double log2(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( x <= 0.0 )
			return kmath::nan("");

		// Use the relationship: log2(x) = ln(x) / ln(2)
		return kmath::log(x) / K_M_LN2;
	}
}  // namespace kmath

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
#include <klog10.h>
#include <kmath.h>

namespace kmath {
	double log10(double x) {
		// Handle special cases
		if( kmath::isnan(x) )
			return kmath::nan("");
		if( x <= 0.0 )
			return kmath::nan("");

		// Use the relationship: log10(x) = ln(x) / ln(10)
		return kmath::log(x) / kmath::log(10);
	}
}  // namespace kmath

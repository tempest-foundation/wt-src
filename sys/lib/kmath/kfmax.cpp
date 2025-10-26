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
#include <kfmax.h>
#include <kmath.h>

namespace kmath {
	double fmax(double x, double y) {
		// Handle NaN cases
		if( x != x )
			return y;
		if( y != y )
			return x;

		return x > y ? x : y;
	}
}  // namespace kmath

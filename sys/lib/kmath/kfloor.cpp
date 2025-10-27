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
#include <kfloor.hpp>
#include <kmath.hpp>

namespace kmath {
	double floor(double x) {
		if( x >= 0.0 ) {
			// For positive numbers, truncate
			return (double) ((long long) x);
		} else {
			// For negative numbers, round down
			long long int_part = (long long) x;
			if( x == (double) int_part ) {
				return x;
			}
			return (double) (int_part - 1);
		}
	}
}  // namespace kmath

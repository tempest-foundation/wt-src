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
#include <kceil.h>
#include <kmath.h>

namespace kmath {
	double ceil(double x) {
		if( x <= 0.0 ) {
			// For negative numbers, truncate
			return (double) ((long long) x);
		} else {
			// For positive numbers, round up
			long long int_part = (long long) x;
			if( x == (double) int_part ) {
				return x;
			}
			return (double) (int_part + 1);
		}
	}
}  // namespace kmath

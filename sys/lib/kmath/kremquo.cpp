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
#include <kmath.hpp>
#include <kremquo.hpp>

namespace kmath {
	double remquo(double x, double y, int *quo) {
		// Handle special cases
		if( kmath::isnan(x) || kmath::isnan(y) ) {
			*quo = 0;
			return nan("");
		}
		if( kmath::isinf(x) ) {
			*quo = 0;
			return kmath::nan("");
		}
		if( y == 0.0 ) {
			*quo = 0;
			return kmath::nan("");
		}
		if( kmath::isinf(y) ) {
			*quo = 0;
			return x;
		}

		// Calculate quotient
		*quo = (int) kmath::round(x / y);

		// Calculate remainder
		double r = x - *quo * y;

		// Adjust to get remainder in the range [-|y|/2, |y|/2]
		if( kmath::fabs(r) > kmath::fabs(y) / 2.0 ) {
			if( r > 0 ) {
				r -= kmath::fabs(y);
				(*quo)++;
			} else {
				r += kmath::fabs(y);
				(*quo)--;
			}
		}

		return r;
	}
}  // namespace kmath

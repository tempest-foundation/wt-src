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
#include <ktan.h>

namespace kmath {
	double tan(double x) {
		double cos_x = kmath::cos(x);
		if( cos_x == 0.0 ) {
			// Return infinity for undefined values
			return 1.0 / 0.0;
		}
		return kmath::sin(x) / cos_x;
	}
}  // namespace kmath

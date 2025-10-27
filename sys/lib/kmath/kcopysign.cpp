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
#include <kcopysign.hpp>
#include <kmath.hpp>

namespace kmath {
	double copysign(double x, double y) {
		if( y >= 0.0 ) {
			return kmath::fabs(x);
		} else {
			return -kmath::fabs(x);
		}
	}
}  // namespace kmath

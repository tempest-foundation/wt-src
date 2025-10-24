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
#include <kround.h>

namespace kmath {
	double round(double x) {
		if( x >= 0.0 ) {
			return kmath::floor(x + 0.5);
		} else {
			return kmath::ceil(x - 0.5);
		}
	}
}  // namespace kmath

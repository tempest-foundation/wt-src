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
#include <kisfinite.hpp>
#include <kmath.hpp>

namespace kmath {
	int isfinite(double x) {
		return !kmath::isnan(x) && !kmath::isinf(x);
	}
}  // namespace kmath

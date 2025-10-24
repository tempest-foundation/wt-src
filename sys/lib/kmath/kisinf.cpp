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
#include <kisinf.h>
#include <kmath.h>

namespace kmath {
	int isinf(double x) {
		return x == 1.0 / 0.0 || x == -1.0 / 0.0;
	}
}  // namespace kmath

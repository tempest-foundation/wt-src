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
#include <ksqrt.h>

namespace kmath {
	double sqrt(double x) {
#ifdef ARCH_AMD64
		double result;
		__asm__ volatile("sqrtsd %1, %0" : "=x"(result) : "x"(x));
		return result;
#endif
	}
}  // namespace kmath

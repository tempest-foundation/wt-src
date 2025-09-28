// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/klog10.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <lib/kmath/klog10.h>
#include <lib/kmath/kmath.h>

double
    klog10 (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (x <= 0.0)
		return knan("");

	// Use the relationship: log10(x) = ln(x) / ln(10)
	return klog(x) / klog(10);
}

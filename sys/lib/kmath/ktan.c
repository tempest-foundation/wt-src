// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/ktan.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmath.h>
#include <ktan.h>

double
    ktan (double x) {
	double cos_x = kcos(x);
	if (cos_x == 0.0) {
		// Return infinity for undefined values
		return 1.0 / 0.0;
	}
	return ksin(x) / cos_x;
}

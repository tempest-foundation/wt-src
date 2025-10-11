// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kmath/kisfinite.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kisfinite.h>
#include <kmath.h>

int
    kisfinite (double x) {
	return !kisnan(x) && !kisinf(x);
}

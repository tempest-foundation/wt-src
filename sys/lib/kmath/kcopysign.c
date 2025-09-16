// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/lib/kmath/kcopysign.c
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <lib/kmath/kcopysign.h>
#include <lib/kmath/kmath.h>

double
    kcopysign (double x, double y) {
	if (y >= 0.0) {
		return kfabs(x);
	} else {
		return -kfabs(x);
	}
}

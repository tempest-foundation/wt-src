// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstdlib/katoi.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <katoi.h>
#include <kctype.h>
#include <kerrno.h>
#include <klimits.h>
#include <kstdlib.h>

int
    katoi (const char *s) {
	long result = 0;
	int  sign   = 1;

	kerrno = 0;

	while (kisspace(*s))
		s++;

	if (*s == '+' || *s == '-') {
		if (*s == '-')
			sign = -1;
		s++;
	}

	if (!kisdigit(*s)) {
		kerrno = KEINVAL;
		return 0;
	}

	while (kisdigit(*s)) {
		int digit = *s - '0';

		if (result > (KLONG_MAX - digit) / 10) {
			kerrno = KERANGE;
			return sign == 1 ? KINT_MAX : KINT_MIN;
		}

		result = result * 10 + digit;
		s++;
	}

	result *= sign;

	if (result < KINT_MIN || result > KINT_MAX) {
		kerrno = KERANGE;
		return result > 0 ? KINT_MAX : KINT_MIN;
	}

	return (int) result;
}

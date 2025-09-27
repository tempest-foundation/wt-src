// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kstrrchr.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <lib/kstdio/kstddef.h>
#include <lib/kstring/kstrrchr.h>

const char *
    kstrrchr (const char *str, int c) {
	if (!str)
		return KNULL;

	const char  ch   = (char) c;
	const char *last = KNULL;

	do {
		if (*str == ch)
			last = str;
	} while (*str++);

	return last;
}

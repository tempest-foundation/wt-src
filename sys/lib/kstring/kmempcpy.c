// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kmempcpy.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmempcpy.h>
#include <kstddef.h>

void *
    kmempcpy (void *dest, const void *src, ksize_t n) {
	char       *d = (char *) dest;
	const char *s = (const char *) src;
	for (ksize_t i = 0; i < n; i++)
		d[i] = s[i];
	return d + n;
}

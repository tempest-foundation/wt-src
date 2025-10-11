// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kmemcmp.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kmemcmp.h>
#include <kstddef.h>

int
    kmemcmp (const void *s1, const void *s2, ksize_t n) {
	const unsigned char *a = s1;
	const unsigned char *b = s2;
	for (ksize_t i = 0; i < n; ++i) {
		if (a[i] != b[i])
			return (int) a[i] - (int) b[i];
	}
	return 0;
}

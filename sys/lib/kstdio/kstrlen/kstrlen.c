// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstdio/kstrlen/kstrlen.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstddef.h>
#include <kstrlen.h>

ksize_t
    kstrlen (const char *str) {
	if (!str)
		return 0;

	const char *s = str;
	while (*s)
		s++;
	return (ksize_t) (s - str);
}

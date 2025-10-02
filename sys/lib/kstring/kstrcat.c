// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kstrcat.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstrcat.h>

char *
    kstrcat (char *dest, const char *src) {
	if (!dest || !src)
		return dest;

	// Find the end of dest
	char *end = dest;
	while (*end) {
		end++;
	}

	// Copy src to the end of dest
	while (*src) {
		*end = *src;
		end++;
		src++;
	}

	*end = '\0';
	return dest;
}

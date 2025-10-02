// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kstrncpy.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstddef.h>
#include <kstrncpy.h>

char *
    kstrncpy (char *dest, const char *src, ksize_t n) {
	char       *d = dest;
	const char *s = src;

	while (n && *s) {
		*d++ = *s++;
		n--;
	}

	while (n--) {
		*d++ = '\0';
	}

	return dest;
}

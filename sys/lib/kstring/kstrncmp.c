// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kstrncmp.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstddef.h>
#include <kstrncmp.h>

int
    kstrncmp (const char *s1, const char *s2, ksize_t n) {
	for (ksize_t i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return (unsigned char) s1[i] - (unsigned char) s2[i];
		}
		if (s1[i] == '\0') {
			return 0;
		}
	}
	return 0;
}

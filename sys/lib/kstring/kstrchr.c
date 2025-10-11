// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kstring/kstrchr.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstddef.h>
#include <kstrchr.h>

char *
    kstrchr (const char *str, int c) {
	while (*str != '\0') {
		if (*str == (char) c) {
			return (char *) str;
		}
		str++;
	}

	if (c == '\0') {
		return (char *) str;
	}

	return KNULL;
}

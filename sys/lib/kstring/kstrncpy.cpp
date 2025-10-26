// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#include <kstddef.h>
#include <kstrncpy.h>

namespace kstring {
	char *strncpy(char *dest, const char *src, size_t n) {
		char       *d = dest;
		const char *s = src;

		while( n && *s ) {
			*d++ = *s++;
			n--;
		}

		while( n-- ) {
			*d++ = '\0';
		}

		return dest;
	}
}  // namespace kstring

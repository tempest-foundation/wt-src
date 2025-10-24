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
#include <kstrcat.h>

namespace string {
	char *strcat(char *dest, const char *src) {
		if( !dest || !src )
			return dest;

		// Find the end of dest
		char *end = dest;
		while( *end ) {
			end++;
		}

		// Copy src to the end of dest
		while( *src ) {
			*end = *src;
			end++;
			src++;
		}

		*end = '\0';
		return dest;
	}
}  // namespace string

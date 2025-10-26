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
#include <kstrncmp.h>

namespace kstring {
	int strncmp(const char *s1, const char *s2, size_t n) {
		for( size_t i = 0; i < n; i++ ) {
			if( s1[i] != s2[i] ) {
				return (unsigned char) s1[i] - (unsigned char) s2[i];
			}
			if( s1[i] == '\0' ) {
				return 0;
			}
		}
		return 0;
	}
}  // namespace kstring

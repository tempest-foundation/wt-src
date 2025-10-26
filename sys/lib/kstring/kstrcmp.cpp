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
#include <kstrcmp.h>

namespace kstring {
	int strcmp(const char *s1, const char *s2) {
		while( *s1 && (*s1 == *s2) ) {
			s1++;
			s2++;
		}
		return *(const unsigned char *) s1 - *(const unsigned char *) s2;
	}
}  // namespace kstring

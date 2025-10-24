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
#include <kmemcmp.h>
#include <kstddef.h>

namespace string {
	int memcmp(const void *s1, const void *s2, size_t n) {
		const unsigned char *a = static_cast<const unsigned char *>(s1);
		const unsigned char *b = static_cast<const unsigned char *>(s2);
		for( size_t i = 0; i < n; ++i ) {
			if( a[i] != b[i] )
				return (int) a[i] - (int) b[i];
		}
		return 0;
	}
}  // namespace string

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

namespace string {
	void *memcpy(void *dest, const void *src, size_t n) {
		if( !dest || !src || n == 0 )
			return dest;

		unsigned char       *d = static_cast<unsigned char *>(dest);
		const unsigned char *s = static_cast<const unsigned char *>(src);

		if( s < d && s + n > d ) {
			for( size_t i = n; i > 0; --i )
				d[i - 1] = s[i - 1];
		} else {
			for( size_t i = 0; i < n; ++i )
				d[i] = s[i];
		}

		return dest;
	}
}  // namespace string

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
#include <kmempcpy.h>
#include <kstddef.h>

namespace string {
	void *mempcpy(void *dest, const void *src, size_t n) {
		char       *d = (char *) dest;
		const char *s = (const char *) src;
		for( size_t i = 0; i < n; i++ )
			d[i] = s[i];
		return d + n;
	}
}  // namespace string

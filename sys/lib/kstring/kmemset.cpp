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
#include <kstdint.h>

namespace string {
	void *memset(void *s, int c, size_t n) {
		if( !s )
			return nullptr;

		unsigned char *byte_ptr = static_cast<unsigned char *>(s);

		for( size_t i = 0; i < n; ++i ) {
			byte_ptr[i] = static_cast<unsigned char>(c);
		}

		return s;
	}
}  // namespace string

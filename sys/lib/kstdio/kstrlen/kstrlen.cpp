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
#include <kstddef.hpp>
#include <kstrlen.hpp>

namespace kstd {
	size_t strlen(const char *str) {
		if( !str )
			return 0;

		const char *s = str;
		while( *s )
			s++;
		return (size_t) (s - str);
	}
}  // namespace kstd

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
#include <kstrrchr.h>

namespace string {
	const char *strrchr(const char *str, int c) {
		if( !str )
			return nullptr;

		const char  ch   = (char) c;
		const char *last = nullptr;

		do {
			if( *str == ch )
				last = str;
		} while( *str++ );

		return last;
	}
}  // namespace string

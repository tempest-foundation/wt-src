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
#include <kstrnlen.h>

namespace kstd {
	size_t strnlen(const char *str, size_t maxlen) {
		size_t len = 0;
		while( len < maxlen && str[len] != '\0' ) {
			len++;
		}
		return len;
	}
}  // namespace kstd

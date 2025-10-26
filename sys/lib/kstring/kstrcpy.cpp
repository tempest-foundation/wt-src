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
#include <kmemcpy.h>
#include <kstrcpy.h>

#include <kstrlen/kstrlen.h>

namespace kstring {
	char *strcpy(char *dest, const char *src) {
		size_t len = kstd::strlen(src);
		memcpy(dest, src, len + 1);  // +1 for null terminator
		return dest;
	}
}  // namespace kstring

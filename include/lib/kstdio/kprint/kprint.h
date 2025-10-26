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
#pragma once

#include <kstdarg.h>
#include <kstddef.h>
#include <kstdint.h>

namespace kstd {
	void  puthex(uint64_t n);
	void  putdec(uint32_t n);
	char *format_double(char *buf, size_t bufsize, double value, int precision);
	void  puts(const char *s);
	int   vsnprintf(char *buffer, size_t size, const char *format, va_list args);
	int   snprintf(char *buffer, size_t size, const char *format, ...);
	int   printf(const char *format, ...);
	void  putchar(int c);
}  // namespace kstd

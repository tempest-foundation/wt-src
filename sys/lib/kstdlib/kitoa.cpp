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
#include <kcharset.hpp>
#include <kitoa.hpp>
#include <kstdlib.hpp>

namespace kstd {
	char *itoa(char *buf, char *end, long value, int base, int is_uppercase) {
		if( base <= 1 ) {
			if( buf < end )
				*buf = '\0';
			return buf;
		}
		const char *digit_set = is_uppercase ? kdigits_upper : kdigits_lower;
		char       *rev       = buf;
		int         negative  = 0;

		// Ensure we have at least 2 bytes for null terminator
		if( buf >= end ) {
			if( buf < end )
				*buf = '\0';
			return buf;
		}

		if( value == 0 ) {
			*rev++ = '0';
			*rev   = '\0';
			return rev;
		}

		if( value < 0 ) {
			negative = 1;
			value    = -value;
		}

		while( value != 0 && rev < end - 1 ) {
			*rev++ = digit_set[(unsigned) (value % (unsigned) base)];
			value /= (unsigned) base;
		}

		if( negative && rev < end - 1 )
			*rev++ = '-';

		*rev = '\0';

		// Reverse string
		for( char *p1 = buf, *p2 = rev - 1; p1 < p2; p1++, p2-- ) {
			char tmp = *p1;
			*p1      = *p2;
			*p2      = tmp;
		}

		return rev;
	}
}  // namespace kstd

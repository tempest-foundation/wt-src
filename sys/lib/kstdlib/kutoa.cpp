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
#include <kstdlib.hpp>
#include <kutoa.hpp>

namespace kstd {
	char *
	    utoa(char *buf, char *end, unsigned long value, int base, int is_uppercase) {
		if( base <= 1 ) {
			if( buf < end )
				*buf = '\0';
			return buf;
		}
		const char *digit_set = is_uppercase ? kdigits_upper : kdigits_lower;
		char       *rev       = buf;

		if( value == 0 ) {
			if( buf < end ) {
				*rev++ = '0';
			}
			return rev;
		}

		while( value != 0 && rev < end ) {
			*rev++ = digit_set[(unsigned) (value % (unsigned) base)];
			value /= (unsigned) base;
		}

		// Reverse string
		for( char *p1 = buf, *p2 = rev - 1; p1 < p2; p1++, p2-- ) {
			char tmp = *p1;
			*p1      = *p2;
			*p2      = tmp;
		}

		return rev;
	}
}  // namespace kstd

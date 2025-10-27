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
#include <kctype.hpp>
#include <kerrno.hpp>
#include <klimits.hpp>
#include <kstdlib.hpp>
#include <kstrtol.hpp>

namespace kstd {
	long strtol(const char *nptr, char **endptr, int base) {
		const char *s   = nptr;
		long        acc = 0;
		int         c;
		int         neg = 0;
		long        cutoff;
		int         cutlim;

		// skip whitespace
		while( ctype::isspace((unsigned char) *s) )
			s++;

		// sign
		if( *s == '-' ) {
			neg = 1;
			s++;
		} else if( *s == '+' ) {
			s++;
		}

		// detect base if 0
		if( base == 0 ) {
			if( *s == '0' ) {
				if( s[1] == 'x' || s[1] == 'X' ) {
					base = 16;
					s += 2;
				} else {
					base = 8;
					s++;
				}
			} else {
				base = 10;
			}
		} else if( base == 16 ) {
			if( s[0] == '0' && (s[1] == 'x' || s[1] == 'X') ) {
				s += 2;
			}
		}

		// compute cutoff for overflow
		cutoff = neg ? KLONG_MIN : KLONG_MAX;
		cutlim = (int) (cutoff % base);
		cutoff /= base;
		if( cutlim < 0 ) {
			cutlim += base;
			cutoff += 1;
		}

		// convert digits
		for( ;; s++ ) {
			c = (unsigned char) *s;
			if( ctype::isdigit(c) )
				c -= '0';
			else if( ctype::isalpha(c) )
				c = ctype::toupper(c) - 'A' + 10;
			else
				break;
			if( c >= base )
				break;

			// check overflow
			if( neg ) {
				if( acc < cutoff || (acc == cutoff && c > cutlim) ) {
					acc   = KLONG_MIN;
					errno = KERANGE;
					neg   = 0;  // to avoid negating again
				} else {
					acc *= base;
					acc -= c;
				}
			} else {
				if( acc > cutoff || (acc == cutoff && c > cutlim) ) {
					acc   = KLONG_MAX;
					errno = KERANGE;
					neg   = 0;  // to avoid negating again
				} else {
					acc *= base;
					acc += c;
				}
			}
		}

		if( endptr )
			*endptr = (char *) (acc == 0 && s == nptr ? nptr : s);

		return acc;
	}
}  // namespace kstd

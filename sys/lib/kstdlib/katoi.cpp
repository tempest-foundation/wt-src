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
#include <katoi.hpp>
#include <kctype.hpp>
#include <kerrno.hpp>
#include <klimits.hpp>
#include <kstdlib.hpp>

namespace kstd {
	int atoi(const char *s) {
		long result = 0;
		int  sign   = 1;

		errno = 0;

		while( ctype::isspace(*s) )
			s++;

		if( *s == '+' || *s == '-' ) {
			if( *s == '-' )
				sign = -1;
			s++;
		}

		if( !ctype::isdigit(*s) ) {
			errno = KEINVAL;
			return 0;
		}

		while( ctype::isdigit(*s) ) {
			int digit = *s - '0';

			if( result > (KLONG_MAX - digit) / 10 ) {
				errno = KERANGE;
				return sign == 1 ? KINT_MAX : KINT_MIN;
			}

			result = result * 10 + digit;
			s++;
		}

		result *= sign;

		if( result < KINT_MIN || result > KINT_MAX ) {
			errno = KERANGE;
			return result > 0 ? KINT_MAX : KINT_MIN;
		}

		return (int) result;
	}
}  // namespace kstd

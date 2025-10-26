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
#include <kstrchr.h>
#include <kstrtok.h>

namespace kstring {
	static char *strtok_save = nullptr;

	char *strtok(char *str, const char *delim) {
		if( str != nullptr ) {
			strtok_save = str;
		} else if( strtok_save == nullptr ) {
			return nullptr;
		}

		// Skip leading delimiters
		while( *strtok_save && strchr(delim, *strtok_save) ) {
			strtok_save++;
		}

		if( *strtok_save == '\0' ) {
			strtok_save = nullptr;
			return nullptr;
		}

		char *token_start = strtok_save;

		// Find end of token
		while( *strtok_save && !strchr(delim, *strtok_save) ) {
			strtok_save++;
		}

		if( *strtok_save != '\0' ) {
			*strtok_save = '\0';
			strtok_save++;
		} else {
			strtok_save = nullptr;
		}

		return token_start;
	}
}  // namespace kstring

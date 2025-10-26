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

namespace ctype {
	int isupper(int c);
	int islower(int c);
	int isalpha(int c);
	int isdigit(int c);
	int isxdigit(int c);
	int isalnum(int c);
	int isspace(int c);
	int isblank(int c);
	int iscntrl(int c);
	int isgraph(int c);
	int isprint(int c);
	int ispunct(int c);
	int tolower(int c);
	int toupper(int c);
}  // namespace ctype

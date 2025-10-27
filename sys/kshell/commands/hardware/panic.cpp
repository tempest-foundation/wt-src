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
#include "panic.h"

#include <katoi.h>
#include <kprint.h>

#include <kern/panic/panic.h>

void cmd_panic(const char *args) {
	if( args == nullptr || *args == '\0' ) {
		kstd::puts("Usage: kpanic <error_code>");
		kstd::puts("Error codes: 0-16 (0=unknown, 1=div_by_zero, etc.)");
		return;
	}

	int code = kstd::atoi(args);
	if( code < 0 || code > 16 ) {
		kstd::puts("Error code must be between 0 and 16");
		return;
	}

	panic::init(code, nullptr);
}

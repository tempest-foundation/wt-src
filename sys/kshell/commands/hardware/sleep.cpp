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
#include "sleep.hpp"

#include <katoi.hpp>
#include <kprint.hpp>
#include <ksleep.hpp>

void cmd_sleep(const char *args) {
	if( args && *args ) {
		unistd::sleep::sleep(kstd::atoi(args));
	} else {
		kstd::puts("Zzzzzzzzzzzzzzzzzzz...");
		kstd::puts("Use: sleep <time in milliseconds>");
	}
}

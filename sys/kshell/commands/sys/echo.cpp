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
#include "echo.hpp"

#include <kstdio.hpp>

#include <drv/tty/tty.hpp>

void cmd_echo(const char *args) {
	if( !args || *args == '\0' ) {
		kstd::puts("Echo... echo... echo...");
		return;
	}
	kstd::puts(args);
	kstd::puts("\n");
}

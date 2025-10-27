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
#include "clear.h"

#include <kerrno.h>
#include <kstdio.h>
#include <kstrtol.h>

#include <drv/video/video.h>

void cmd_clear(const char *args) {
	uint32_t color = 0x000000;
	if( args && *args ) {
		int base = 0;
		if( args[0] == '#' ) {
			args++;
			base = 16;
		}
		errno = 0;
		char *end;
		long  val = kstd::strtol(args, &end, base);
		if( errno == 0 && end != args && val >= 0 && val <= 0xFFFFFF )
			color = (uint32_t) val;
	}
	video::clear(color);
}

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
#include "fetch.hpp"

#include <kstdio.hpp>
#include <ktime.hpp>

#include <kern/framebuf/framebuf.hpp>
#include <kern/memory/memory.hpp>

void cmd_fetch(const char *args) {
	(void) args;
	const char *ascii[] = {"@                         @",
	                       " @@           @       @@@@",
	                       "     @@@@@@@           ",
	                       "@                        @@",
	                       "   @@@@@@             @ ",
	                       "            @@@@@@@@     ",
	                       "@                         @",
	                       " @     @@@@@@@@@@      @@ "};
	char        info[8][96];
	kstd::snprintf(info[0], sizeof(info[0]), "os: wind");
	kstd::snprintf(info[1], sizeof(info[1]), "kernel: tempest");
	extern char cpu_brand_string[49];
	kstd::snprintf(info[2], sizeof(info[2]), "cpu: %s", cpu_brand_string);
	kstd::snprintf(
	    info[3], sizeof(info[3]), "uptime: %.8f", time::get_uptime_precise());

	if( fb_info.width && fb_info.height && fb_info.bpp ) {
		kstd::snprintf(info[4],
		               sizeof(info[4]),
		               "resolution : %ux%u %ubpp",
		               fb_info.width,
		               fb_info.height,
		               (unsigned int) fb_info.bpp);
	} else {
		kstd::snprintf(info[4], sizeof(info[4]), "resolution: unknown");
	}
	memory_stats_t stats    = memory::stats::get();
	uint64_t       total_kb = stats.total_physical_pages * 4096 / 1024;
	uint64_t       used_kb  = stats.used_physical_pages * 4096 / 1024;
	uint64_t       free_kb  = stats.free_physical_pages * 4096 / 1024;
	uint64_t       total_mb = total_kb / 1024;
	uint64_t       used_mb  = used_kb / 1024;
	uint64_t       free_mb  = free_kb / 1024;
	if( total_mb >= 1 ) {
		kstd::snprintf(info[5],
		               sizeof(info[5]),
		               "memory: %llu MB used / %llu MB total (%llu MB free)",
		               used_mb,
		               total_mb,
		               free_mb);
	} else {
		kstd::snprintf(info[5],
		               sizeof(info[5]),
		               "memory: %llu kB used / %llu kB total (%llu kB free)",
		               used_kb,
		               total_kb,
		               free_kb);
	}
	info[5][0] = '\0';
	info[6][0] = '\0';
	info[7][0] = '\0';

	for( int i = 0; i < 8; ++i ) {
		if( info[i][0] )
			kstd::printf("%-28s  %s\n", ascii[i], info[i]);
		else
			kstd::printf("%s\n", ascii[i]);
	}
}

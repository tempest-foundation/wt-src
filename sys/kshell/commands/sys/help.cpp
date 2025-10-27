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
#include "help.h"

#include <kstdio.h>
#include <kstring.h>

#include <drv/tty/tty.h>
#include <kshell/commands/commands.h>

void cmd_help(const char *args) {
	(void) args;
	kstd::printf("Available commands\n");

	// collect categories
	const char *categories[16];
	int         num_categories = 0;
	for( size_t i = 0; i < NUM_COMMANDS; ++i ) {
		int found = 0;
		for( int j = 0; j < num_categories; ++j ) {
			if( kstring::strcmp(commands[i].cat, categories[j]) == 0 ) {
				found = 1;
				break;
			}
		}
		if( !found )
			categories[num_categories++] = commands[i].cat;
	}

	char buf[128];
	for( int c = 0; c < num_categories; ++c ) {
		kstd::snprintf(buf, sizeof(buf), "\n[%s]\n", categories[c]);
		kstd::printf(buf);
		for( size_t i = 0; i < NUM_COMMANDS; ++i ) {
			if( kstring::strcmp(commands[i].cat, categories[c]) == 0 ) {
				kstd::snprintf(buf,
				               sizeof(buf),
				               " %-12s - %s\n",
				               commands[i].name,
				               commands[i].desc);
				kstd::printf(buf);
			}
		}
	}
}

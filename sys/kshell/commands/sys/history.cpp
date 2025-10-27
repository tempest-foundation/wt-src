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
#include "history.h"

#include <kstdio.h>

#include <drv/tty/tty.h>
#include <kshell/commands/commands.h>

char command_history[MAX_HISTORY][CMD_BUFFER_SIZE];
int  history_count = 0;

void cmd_history(const char *args) {
	(void) args;
	if( history_count == 0 ) {
		kstd::puts("No commands in history\n");
		return;
	}
	char buf[128];
	for( int i = 0; i < history_count && i < MAX_HISTORY; ++i ) {
		kstd::snprintf(buf, sizeof(buf), "%2d - %s\n", i + 1, command_history[i]);
		kstd::puts(buf);
	}
}

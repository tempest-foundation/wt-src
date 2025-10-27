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
#include <kprint.h>
#include <ktime.h>

void cmd_time(const char *args) {
	(void) args;

	// Buffer for date and time strings
	char date_buffer[16];
	char time_buffer[16];

	// Retrieve date and time strings
	time::get_date_string(date_buffer, sizeof(date_buffer));
	time::get_time_string(time_buffer, sizeof(time_buffer));

	kstd::printf("Date: %s\n", date_buffer);
	kstd::printf("Time: %s\n", time_buffer);
}

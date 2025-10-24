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

#include <kstdint.h>

// BIOS time structure
struct bios_time {
	uint8_t  second;
	uint8_t  minute;
	uint8_t  hour;
	uint8_t  day;
	uint8_t  month;
	uint16_t year;
	uint8_t  day_of_week;
};

namespace time {
	void        get_bios(struct bios_time *bios_time_val);
	void        get_date_string(char *buffer, size_t buffer_size);
	void        get_time_string(char *buffer, size_t buffer_size);
	void        get_datetime_string(char *buffer, size_t buffer_size);
	const char *get_day_of_week_string(uint8_t day_of_week);
}  // namespace time

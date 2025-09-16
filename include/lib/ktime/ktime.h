// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        include/lib/ktime/ktime.h
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#pragma once

#include <lib/kstdio/kstdint.h>

extern struct Time time;

// BIOS time structure
struct bios_time {
	kuint8_t  second;
	kuint8_t  minute;
	kuint8_t  hour;
	kuint8_t  day;
	kuint8_t  month;
	kuint16_t year;
	kuint8_t  day_of_week;
};

struct Time {
	void (*get_bios)(struct bios_time *time);
	void (*get_date_str)(char *buffer, ksize_t buffer_size);
	void (*get_time_str)(char *buffer, ksize_t buffer_size);
	void (*get_datetime_str)(char *buffer, ksize_t buffer_size);
	const char *(*get_day_of_week_str)(kuint8_t day_of_week);
};

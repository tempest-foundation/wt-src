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

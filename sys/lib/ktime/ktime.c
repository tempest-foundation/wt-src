// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/lib/ktime/ktime.c
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <arch/amd64/kasm/kio.h>
#include <lib/kstdio/kstdio.h>
#include <lib/ktime/ktime.h>

// BIOS interrupt for getting RTC (Real Time Clock) data
#define RTC_COMMAND_PORT 0x70
#define RTC_DATA_PORT    0x71

// RTC register addresses
#define RTC_SECONDS      0x00
#define RTC_MINUTES      0x02
#define RTC_HOURS        0x04
#define RTC_DAY_OF_WEEK  0x06
#define RTC_DAY_OF_MONTH 0x07
#define RTC_MONTH        0x08
#define RTC_YEAR         0x09
#define RTC_CENTURY      0x32

// RTC status register B
#define RTC_STATUS_B 0x0B

// BCD to decimal conversion
static kuint8_t
    kbcd_to_decimal (kuint8_t bcd) {
	return (kuint8_t) (((bcd >> 4) * 10) + (bcd & 0x0F));
}

// Read a byte from RTC register
static kuint8_t
    krtc_read (kuint8_t reg) {
	koutb(RTC_COMMAND_PORT, reg);
	return kinb(RTC_DATA_PORT);
}

// Check if RTC is in BCD mode
static kbool
    krtc_is_bcd (void) {
	kuint8_t status_b = krtc_read(RTC_STATUS_B);
	return !(status_b & 0x04);  // Bit 2 clear means BCD mode
}

// Get current BIOS time
void
    time_get_bios (struct bios_time *bios_time_val) {
	if (!bios_time_val)
		return;

	kbool is_bcd = krtc_is_bcd();

	// Read all time components
	kuint8_t seconds     = krtc_read(RTC_SECONDS);
	kuint8_t minutes     = krtc_read(RTC_MINUTES);
	kuint8_t hours       = krtc_read(RTC_HOURS);
	kuint8_t day         = krtc_read(RTC_DAY_OF_MONTH);
	kuint8_t month       = krtc_read(RTC_MONTH);
	kuint8_t year        = krtc_read(RTC_YEAR);
	kuint8_t century     = krtc_read(RTC_CENTURY);
	kuint8_t day_of_week = krtc_read(RTC_DAY_OF_WEEK);

	// Convert BCD to decimal if needed
	if (is_bcd) {
		bios_time_val->second      = kbcd_to_decimal(seconds);
		bios_time_val->minute      = kbcd_to_decimal(minutes);
		bios_time_val->hour        = kbcd_to_decimal(hours);
		bios_time_val->day         = kbcd_to_decimal(day);
		bios_time_val->month       = kbcd_to_decimal(month);
		bios_time_val->year        = (kuint16_t) (kbcd_to_decimal(year)
                                                   + (kbcd_to_decimal(century) * 100));
		bios_time_val->day_of_week = kbcd_to_decimal(day_of_week);
	} else {
		bios_time_val->second      = seconds;
		bios_time_val->minute      = minutes;
		bios_time_val->hour        = hours;
		bios_time_val->day         = day;
		bios_time_val->month       = month;
		bios_time_val->year        = (kuint16_t) (year + (century * 100));
		bios_time_val->day_of_week = day_of_week;
	}

	// Handle 12-hour format (bit 6 of hours register indicates PM)
	if (!(hours & 0x80) && (hours & 0x40)) {
		// 12-hour format, PM
		bios_time_val->hour = (kuint8_t) (((bios_time_val->hour % 12) + 12) % 24);
	} else if (!(hours & 0x80)) {
		// 12-hour format, AM
		bios_time_val->hour = bios_time_val->hour % 12;
	}
}

// Get date string in DD-MM-YYYY format
void
    time_get_date_string (char *buffer, ksize_t buffer_size) {
	if (!buffer || buffer_size < 11)
		return;

	struct bios_time bios_time_val;
	time_get_bios(&bios_time_val);

	// Format: DD-MM-YYYY
	ksnprintf(buffer,
	          buffer_size,
	          "%02d/%02d/%04d",
	          bios_time_val.day,
	          bios_time_val.month,
	          bios_time_val.year);
}

// Get time string in HH:MM:SS format
void
    time_get_time_string (char *buffer, ksize_t buffer_size) {
	if (!buffer || buffer_size < 9)
		return;

	struct bios_time bios_time_val;
	time_get_bios(&bios_time_val);

	// Format: HH:MM:SS
	ksnprintf(buffer,
	          buffer_size,
	          "%02d:%02d:%02d",
	          bios_time_val.hour,
	          bios_time_val.minute,
	          bios_time_val.second);
}

// Get full date and time string
void
    time_get_datetime_string (char *buffer, ksize_t buffer_size) {
	if (!buffer || buffer_size < 16)
		return;

	struct bios_time bios_time_val;
	time_get_bios(&bios_time_val);

	// Format: DD-MM-YYYY HH:MM
	kprintf("%02d-%02d-%04d %02d:%02d",
	        bios_time_val.day,
	        bios_time_val.month,
	        bios_time_val.year,
	        bios_time_val.hour,
	        bios_time_val.minute);

	// Copy to buffer
	ksnprintf(buffer,
	          buffer_size,
	          "%02d/%02d/%02d %02d:%02d",
	          bios_time_val.day,
	          bios_time_val.month,
	          bios_time_val.year % 100,
	          bios_time_val.hour,
	          bios_time_val.minute);
}

// Get day of week string
const char *
    time_get_day_of_week_string (kuint8_t day_of_week) {
	static const char *days[] = {
	    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	if (day_of_week >= 1 && day_of_week <= 7) {
		return days[day_of_week - 1];
	}
	return "Unknown";
}

struct Time time = {
    .get_bios            = time_get_bios,
    .get_date_str        = time_get_date_string,
    .get_time_str        = time_get_time_string,
    .get_datetime_str    = time_get_datetime_string,
    .get_day_of_week_str = time_get_day_of_week_string,
};

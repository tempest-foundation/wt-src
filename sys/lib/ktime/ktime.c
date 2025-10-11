// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/ktime/ktime.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#ifdef ARCH_AMD64
#	include <arch/amd64/asm/kio.h>
#endif
#include <kstdio.h>
#include <ktime.h>

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

/**
 * @brief Convert a BCD (Binary Coded Decimal) value to decimal
 * @param bcd The BCD value to convert
 * @return The decimal representation of the BCD value
 */
static kuint8_t
    kbcd_to_decimal (kuint8_t bcd) {
	return (kuint8_t) (((bcd >> 4) * 10) + (bcd & 0x0F));
}

/**
 * @brief Read a byte from the specified RTC register
 * @param reg RTC register address
 * @return Value read from the RTC register
 */
static kuint8_t
    krtc_read (kuint8_t reg) {
	koutb(RTC_COMMAND_PORT, reg);
	return kinb(RTC_DATA_PORT);
}

/**
 * @brief Check if the RTC (Real Time Clock) is in BCD mode
 *
 * Reads RTC Status Register B and determines if RTC registers are in
 * BCD (Binary Coded Decimal) format.  If bit 2 of Status Register B is
 * clear, the RTC is operating in BCD mode; otherwise, it is in binary mode.
 *
 * @return kbool Returns true if RTC is in BCD mode, false otherwise
 */
static kbool
    krtc_is_bcd (void) {
	kuint8_t status_b = krtc_read(RTC_STATUS_B);
	return !(status_b & 0x04);  // Bit 2 clear means BCD mode
}

/**
 * @brief Populate a bios_time structure with the current time from the RTC
 *
 * Reads the current date and time values from the hardware Real Time Clock (RTC)
 * using BIOS-compatible registers.  Automatically handles BCD/binary format and
 * 12-hour/24-hour conversion as indicated by RTC status and register values.
 *
 * @param[in,out] bios_time_val Pointer to a struct bios_time that will be filled in.
 *                              Fields set: year, month, day, hour, minute,
 *                              second, day_of_week.
 */
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

/**
 * @brief Get date string in DD-MM-YYYY format
 *
 * Populates the provided buffer with a date string formatted as DD/MM/YYYY,
 * using the current BIOS time.
 *
 * @param[out] buffer       Buffer to store the formatted date string
 * @param[in]  buffer_size  Size of the buffer. Must be at least 11 bytes
 */
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

/**
 * @brief Get time string in HH:MM:SS format
 *
 * Populates the provided buffer with a time string formatted as HH:MM:SS,
 * using the current BIOS time.
 *
 * @param[out] buffer       Buffer to store the formatted time string
 * @param[in]  buffer_size  Size of the buffer. Must be at least 9 bytes
 */
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

/**
 * @brief Get full date and time string
 *
 * Populates the provided buffer with a combined date and time string formatted as DD/MM/YY HH:MM,
 * using the current BIOS time.
 *
 * @param[out] buffer       Buffer to store the formatted date and time string
 * @param[in]  buffer_size  Size of the buffer. Must be at least 16 bytes
 */
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

/**
 * @brief Get day of week string
 *
 * Returns the name of the day corresponding to the given day_of_week number.
 * 1 = Sunday, 2 = Monday, ..., 7 = Saturday.
 *
 * @param[in] day_of_week  Numeric day of the week (1-7)
 * @return                 Name of the day or "Unknown" if out of range
 */
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

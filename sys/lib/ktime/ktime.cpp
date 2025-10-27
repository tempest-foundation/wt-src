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
#ifdef ARCH_AMD64
#	include <arch/amd64/asm/io.hpp>
#endif
#include <kstdio.hpp>
#include <ktime.hpp>

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

namespace time {
	// Uptime tracking variables
	static volatile uint64_t uptime_ticks     = 0;
	static const uint64_t    TICKS_PER_SECOND = 100;  // Assuming 100Hz timer

	/**
 * @brief Convert a BCD (Binary Coded Decimal) value to decimal
 * @param bcd The BCD value to convert
 * @return The decimal representation of the BCD value
 */
	static uint8_t kbcd_to_decimal(uint8_t bcd) {
		return (uint8_t) (((bcd >> 4) * 10) + (bcd & 0x0F));
	}

	/**
 * @brief Read a byte from the specified RTC register
 * @param reg RTC register address
 * @return Value read from the RTC register
 */
	static uint8_t krtc_read(uint8_t reg) {
		outb(RTC_COMMAND_PORT, reg);
		return inb(RTC_DATA_PORT);
	}

	/**
 * @brief Check if the RTC (Real Time Clock) is in BCD mode
 *
 * Reads RTC Status Register B and determines if RTC registers are in
 * BCD (Binary Coded Decimal) format.  If bit 2 of Status Register B is
 * clear, the RTC is operating in BCD mode; otherwise, it is in binary mode.
 *
 * @return bool Returns true if RTC is in BCD mode, false otherwise
 */
	static bool krtc_is_bcd(void) {
		uint8_t status_b = krtc_read(RTC_STATUS_B);
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
	void get_bios(struct bios_time *bios_time_val) {
		if( !bios_time_val )
			return;

		bool is_bcd = krtc_is_bcd();

		// Read all time components
		uint8_t seconds     = krtc_read(RTC_SECONDS);
		uint8_t minutes     = krtc_read(RTC_MINUTES);
		uint8_t hours       = krtc_read(RTC_HOURS);
		uint8_t day         = krtc_read(RTC_DAY_OF_MONTH);
		uint8_t month       = krtc_read(RTC_MONTH);
		uint8_t year        = krtc_read(RTC_YEAR);
		uint8_t century     = krtc_read(RTC_CENTURY);
		uint8_t day_of_week = krtc_read(RTC_DAY_OF_WEEK);

		// Convert BCD to decimal if needed
		if( is_bcd ) {
			bios_time_val->second = kbcd_to_decimal(seconds);
			bios_time_val->minute = kbcd_to_decimal(minutes);
			bios_time_val->hour   = kbcd_to_decimal(hours);
			bios_time_val->day    = kbcd_to_decimal(day);
			bios_time_val->month  = kbcd_to_decimal(month);
			bios_time_val->year =
			    (uint16_t) (kbcd_to_decimal(year)
			                + (kbcd_to_decimal(century) * 100));
			bios_time_val->day_of_week = kbcd_to_decimal(day_of_week);
		} else {
			bios_time_val->second      = seconds;
			bios_time_val->minute      = minutes;
			bios_time_val->hour        = hours;
			bios_time_val->day         = day;
			bios_time_val->month       = month;
			bios_time_val->year        = (uint16_t) (year + (century * 100));
			bios_time_val->day_of_week = day_of_week;
		}

		// Handle 12-hour format (bit 6 of hours register indicates PM)
		if( !(hours & 0x80) && (hours & 0x40) ) {
			// 12-hour format, PM
			bios_time_val->hour =
			    (uint8_t) (((bios_time_val->hour % 12) + 12) % 24);
		} else if( !(hours & 0x80) ) {
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
	void get_date_string(char *buffer, size_t buffer_size) {
		if( !buffer || buffer_size < 11 )
			return;

		struct bios_time bios_time_val;
		get_bios(&bios_time_val);

		// Format: DD-MM-YYYY
		kstd::snprintf(buffer,
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
	void get_time_string(char *buffer, size_t buffer_size) {
		if( !buffer || buffer_size < 9 )
			return;

		struct bios_time bios_time_val;
		get_bios(&bios_time_val);

		// Format: HH:MM:SS
		kstd::snprintf(buffer,
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
	void get_datetime_string(char *buffer, size_t buffer_size) {
		if( !buffer || buffer_size < 16 )
			return;

		struct bios_time bios_time_val;
		get_bios(&bios_time_val);

		// Format: DD-MM-YYYY HH:MM
		kstd::printf("%02d-%02d-%04d %02d:%02d",
		             bios_time_val.day,
		             bios_time_val.month,
		             bios_time_val.year,
		             bios_time_val.hour,
		             bios_time_val.minute);

		// Copy to buffer
		kstd::snprintf(buffer,
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
	const char *get_day_of_week_string(uint8_t day_of_week) {
		static const char *days[] = {"Sunday",
		                             "Monday",
		                             "Tuesday",
		                             "Wednesday",
		                             "Thursday",
		                             "Friday",
		                             "Saturday"};

		if( day_of_week >= 1 && day_of_week <= 7 ) {
			return days[day_of_week - 1];
		}
		return "Unknown";
	}

	/**
     * @brief Initialize the uptime counter
     * 
     * Should be called during kernel initialization before enabling interrupts.
     */
	void uptime_init(void) {
		uptime_ticks = 0;
	}

	/**
     * @brief Increment the uptime tick counter
     * 
     * This should be called from the timer interrupt handler (IRQ0).
     */
	void uptime_tick(void) {
		uptime_ticks = uptime_ticks + 1;
	}

	/**
     * @brief Get the system uptime in seconds
     * 
     * @return Number of seconds the system has been running
     */
	uint64_t get_uptime_seconds(void) {
		return uptime_ticks / TICKS_PER_SECOND;
	}

	double get_uptime_precise(void) {
		uint64_t ticks = uptime_ticks;
		return (double) ticks / (double) TICKS_PER_SECOND;
	}

	/**
     * @brief Get formatted uptime string
     * 
     * Formats uptime as "X days, Y hours, Z minutes, W seconds"
     * 
     * @param[out] buffer       Buffer to store the formatted string
     * @param[in]  buffer_size  Size of the buffer (should be at least 64 bytes)
     */
	void get_uptime_string(char *buffer, size_t buffer_size) {
		if( !buffer || buffer_size < 64 )
			return;

		uint64_t total_seconds = get_uptime_seconds();

		uint64_t days = total_seconds / 86400;
		total_seconds %= 86400;

		uint64_t hours = total_seconds / 3600;
		total_seconds %= 3600;

		uint64_t minutes = total_seconds / 60;
		uint64_t seconds = total_seconds % 60;

		if( days > 0 ) {
			kstd::snprintf(buffer,
			               buffer_size,
			               "%llu day%s, %llu:%02llu:%02llu",
			               days,
			               days == 1 ? "" : "s",
			               hours,
			               minutes,
			               seconds);
		} else {
			kstd::snprintf(buffer,
			               buffer_size,
			               "%llu:%02llu:%02llu",
			               hours,
			               minutes,
			               seconds);
		}
	}
}  // namespace time

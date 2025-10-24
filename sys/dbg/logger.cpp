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
#include <kstdio.h>
#include <kutoa.h>

#include <dbg/logger.h>
#include <drv/serial/serial.h>
#include <drv/video/video.h>

bool d_enabled = false;

namespace logger {
	/**
	 * @brief Debug logger using the kernel puts to log the system
	 * @param subsystem The subsystem that the computer is running
	 * @param type Type of the log, ex: error, warn, emerg, notice and etc...
	 * @param message The message to be show
	 * @note Logging only happens if debug is enabled
	 */

	void puts(const char *subsystem, const char *type, const char *message) {
		if( !d_enabled ) {
			return;
		}
		if( !message || *message == '\0' )
			return;

		kstd::puts("[    debug");

		if( subsystem && *subsystem != '\0' ) {
			kstd::puts("::");
			kstd::puts(subsystem);
		}

		if( type && *type != '\0' ) {
			kstd::puts("/");
			kstd::puts(type);
		}

		kstd::puts("] ");
		kstd::puts(message);
		kstd::putchar('\n');
	}

	int printf(const char *subsystem, const char *type, const char *format, ...) {
		if( !d_enabled )
			return 0;

		if( !format || *format == '\0' )
			return 1;

		va_list args;
		k_va_start(args, format);
		int count = 0;

		// Write debug header with optional subsystem and type
		video::puts("[    debug");
		count += 10;  // Length of "[    debug"

		if( subsystem && *subsystem != '\0' ) {
			video::puts("::");
			video::puts(subsystem);
			count += 2 + (int) kstd::strlen(subsystem);
		}

		if( type && *type != '\0' ) {
			video::puts("/");
			video::puts(type);
			count += 2 + (int) kstd::strlen(type);
		}

		video::puts("] ");
		count += 2;  // Length of "] "

		for( const char *p = format; *p; ++p ) {
			if( *p != '%' ) {
				kstd::putchar(*p);
				count++;
				continue;
			}

			p++;

			int left_align = 0;
			int width      = 0;

			if( *p == '-' ) {
				left_align = 1;
				p++;
			}

			while( *p >= '0' && *p <= '9' ) {
				width = width * 10 + (*p - '0');
				p++;
			}

			switch( *p ) {
				case 's': {
					const char *s   = k_va_arg(args, const char *);
					int         len = 0;
					const char *t   = s;
					while( *t++ )
						len++;

					int pad = (width > len) ? (width - len) : 0;

					if( !left_align ) {
						for( int i = 0; i < pad; ++i ) {
							kstd::putchar(' ');
							count++;
						}
					}

					video::puts(s);
					count += len;

					if( left_align ) {
						for( int i = 0; i < pad; ++i ) {
							kstd::putchar(' ');
							count++;
						}
					}
					break;
				}

				case 'd': {
					int   n = k_va_arg(args, int);
					char  buf[12];
					char *ptr = buf;

					if( n < 0 ) {
						*ptr++ = '-';
						n      = -n;
					}

					char *end_ptr = kstd::utoa(ptr,
					                           buf + sizeof(buf) - 1,
					                           (unsigned int) n,
					                           10,
					                           0);
					*end_ptr      = '\0';
					video::puts(buf);
					count += (int) (end_ptr - buf);
					break;
				}

				case 'x': {
					unsigned int n = k_va_arg(args, unsigned int);
					char         buf[12];
					char        *end_ptr = kstd::utoa(
                                            buf, buf + sizeof(buf) - 1, n, 16, 0);
					*end_ptr = '\0';
					video::puts(buf);
					count += (int) (end_ptr - buf);
					break;
				}
				case 'l': {
					// Handle long/long long modifiers
					if( *(p + 1) == 'l' ) {
						p++;  // Skip second 'l'
						if( *(p + 1) == 'x' ) {
							p++;  // Skip 'x'
							uint64_t n =
							    k_va_arg(args, uint64_t);
							char  buf[20];
							char *end_ptr = kstd::utoa(
							    buf,
							    buf + sizeof(buf) - 1,
							    (unsigned int) (n
							                    & 0xFFFFFFFF),
							    16,
							    0);
							*end_ptr = '\0';
							video::puts(buf);
							count += (int) (end_ptr - buf);
							break;
						}
					}
					// Fall through to default for single 'l'
					goto default_case;
				}

				case 'c': {
					char c = (char) k_va_arg(args, int);
					kstd::putchar(c);
					count++;
					break;
				}

				case '%': {
					kstd::putchar('%');
					count++;
					break;
				}

				default:
				default_case: {
					kstd::putchar('%');
					kstd::putchar(*p);
					count += 2;
					break;
				}
			}
		}

		k_va_end(args);
		return count;
	}

	/**
	 * @brief Logger type used by the logger.type() functions
	 * @param type Type of the situation. Example: error, warn, notice
	 * @param subsystem The current stage that the computer is in
	 * @param message Message to be show
	 * @param extra Extra parameters from the message
	 */
	static void logger_type(const char *type,
	                        const char *subsystem,
	                        const char *message,
	                        const char *extra) {
		if( !message || *message == '\0' )
			return;

		kstd::printf("[    %s", type);

		if( subsystem && *subsystem != '\0' )
			kstd::printf("::%s", subsystem);

		kstd::printf("] %s", message);

		if( extra && *extra != '\0' )
			kstd::printf(": %s", extra);

		kstd::printf("\n");
	}

	void crit(const char *s, const char *m, const char *e) {
		logger_type("crit", s, m, e);
	}

	void alert(const char *s, const char *m, const char *e) {
		logger_type("alert", s, m, e);
	}

	void emerg(const char *s, const char *m, const char *e) {
		logger_type("emerg", s, m, e);
	}

	void warn(const char *s, const char *m, const char *e) {
		logger_type("warn", s, m, e);
	}

	void error(const char *s, const char *m, const char *e) {
		logger_type("error", s, m, e);
	}

	void notice(const char *s, const char *m, const char *e) {
		logger_type("notice", s, m, e);
	}

	void info(const char *s, const char *m, const char *e) {
		logger_type("info", s, m, e);
	}

	void success(const char *s, const char *m, const char *e) {
		logger_type("success", s, m, e);
	}
}  // namespace logger

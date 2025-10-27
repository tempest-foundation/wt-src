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
#include <kstdio.hpp>
#include <ktime.hpp>
#include <kutoa.hpp>

#include <dbg/logger.hpp>
#include <drv/serial/serial.hpp>
#include <drv/video/video.hpp>

bool dbg_enabled = true;

namespace logger {
	namespace debug {
		/**
	 * @brief Debug logger using the kernel puts to log the system
	 * @param subsystem The subsystem that the computer is running
	 * @param type Type of the log, ex: error, warn, emerg, notice and etc...
	 * @param message The message to be show
	 * @note Logging only happens if debug is enabled
	 */

		void puts(const char *subsystem, const char *type, const char *message) {
			if( !dbg_enabled )
				return;
			if( !message || *message == '\0' )
				return;

			serial::write('[');
			char timebuf[32];
			kstd::format_double(
			    timebuf, sizeof(timebuf), time::get_uptime_precise(), 8);
			serial::writes(timebuf);
			serial::writes(", ");
			if( subsystem && *subsystem != '\0' ) {
				serial::write('@');
				serial::writes(subsystem);
				serial::writes(", ");
			}

			serial::writes("DEBUG");

			if( type && *type != '\0' ) {
				serial::writes("/");
				for( const char *t = type; *t; ++t )
					serial::write((*t >= 'a' && *t <= 'z') ? *t - 32
					                                       : *t);
			}

			serial::writes("] ");
			serial::writes(message);
			serial::write('\n');
		}

		/*
	 * This code is so bad...
	 */
		int printf(const char *subsystem,
		           const char *type,
		           const char *format,
		           ...) {
			if( !dbg_enabled )
				return 0;
			va_list args;
			k_va_start(args, format);
			int count = 0;

			serial::write('[');
			// uptime
			char timebuf[32];
			kstd::format_double(
			    timebuf, sizeof(timebuf), time::get_uptime_precise(), 8);
			serial::writes(timebuf);
			serial::writes(", ");
			if( subsystem && *subsystem != '\0' ) {
				serial::write('@');
				serial::writes(subsystem);
				serial::writes(", ");
			}

			serial::writes("DEBUG");

			if( type && *type != '\0' ) {
				serial::write('/');
				for( const char *t = type; *t; ++t )
					serial::write((*t >= 'a' && *t <= 'z') ? *t - 32
					                                       : *t);
			}
			serial::writes("] ");

			for( const char *p = format; *p; ++p ) {
				if( *p != '%' ) {
					serial::write(*p);
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

				// Check for precision (for floats)
				int precision = 6;
				if( *p == '.' ) {
					p++;
					precision = 0;
					while( *p >= '0' && *p <= '9' ) {
						precision = precision * 10 + (*p - '0');
						p++;
					}
				}

				int long_long = 0;
				if( *p == 'l' && *(p + 1) == 'l' ) {
					long_long = 1;
					p += 2;
				}

				switch( *p ) {
					case 's': {
						const char *s =
						    k_va_arg(args, const char *);
						if( !s )
							s = "(null)";
						int         len = 0;
						const char *t   = s;
						while( *t++ )
							len++;
						int pad = (width > len) ? (width - len)
						                        : 0;
						if( !left_align )
							for( int i = 0; i < pad; ++i ) {
								serial::write(' ');
								count++;
							}
						for( int i = 0; i < len; ++i ) {
							serial::write(s[i]);
							count++;
						}
						if( left_align )
							for( int i = 0; i < pad; ++i ) {
								serial::write(' ');
								count++;
							}
						break;
					}

					case 'f': {
						double val = k_va_arg(args, double);
						char   buf[64];
						kstd::format_double(
						    buf, sizeof(buf), val, precision);
						const char *s = buf;
						while( *s ) {
							serial::write(*s++);
							count++;
						}
						break;
					}

					case 'd': {
						if( long_long ) {
							int64_t n =
							    k_va_arg(args, int64_t);
							if( n < 0 ) {
								serial::write('-');
								count++;
								n = -n;
							}
							char buf[21];
							int  idx = 20;
							buf[idx] = '\0';
							if( n == 0 )
								buf[--idx] = '0';
							else {
								uint64_t un =
								    (uint64_t) n;
								while( un ) {
									buf[--idx] =
									    (char) ('0'
									            + (un
									               % 10));
									un /= 10;
								}
							}
							int len = 20 - idx;
							int pad = (width > len)
							              ? width - len
							              : 0;
							if( !left_align )
								for( int i = 0; i < pad;
								     ++i ) {
									serial::write(
									    ' ');
									count++;
								}
							for( int i = idx; i < 20; ++i ) {
								serial::write(buf[i]);
								count++;
							}
							if( left_align )
								for( int i = 0; i < pad;
								     ++i ) {
									serial::write(
									    ' ');
									count++;
								}
						} else {
							int n = k_va_arg(args, int);
							if( n < 0 ) {
								serial::write('-');
								count++;
								n = -n;
							}
							// measure digits to update count
							int temp = n, digits = 1;
							while( temp >= 10 ) {
								temp /= 10;
								digits++;
								serial::write((char) n);
							}
							count += digits;
						}
						break;
					}

					case 'x': {
						if( long_long ) {
							uint64_t n =
							    k_va_arg(args, uint64_t);
							serial::write((char) n);
							uint64_t temp   = n;
							int      digits = 1;
							while( temp >= 16 ) {
								temp /= 16;
								digits++;
							}
							count += digits;
						} else {
							unsigned int n =
							    k_va_arg(args, unsigned int);
							serial::write((char) n);
							unsigned int temp   = n;
							int          digits = 1;
							while( temp >= 16 ) {
								temp /= 16;
								digits++;
							}
							count += digits;
						}
						break;
					}

					case 'c': {
						char c = (char) k_va_arg(args, int);
						serial::write(c);
						count++;
						break;
					}

					case '%': {
						serial::write('%');
						count++;
						break;
					}

					case 'u': {
						if( long_long ) {
							uint64_t n =
							    k_va_arg(args, uint64_t);
							char buf[21];
							int  idx = 20;
							buf[idx] = '\0';
							if( n == 0 )
								buf[--idx] = '0';
							else {
								while( n ) {
									buf[--idx] =
									    (char) ('0'
									            + (n
									               % 10));
									n /= 10;
								}
							}
							int len = 20 - idx;
							int pad = (width > len)
							              ? width - len
							              : 0;
							if( !left_align )
								for( int i = 0; i < pad;
								     ++i ) {
									serial::write(
									    ' ');
									count++;
								}
							for( int i = idx; i < 20; ++i ) {
								serial::write(buf[i]);
								count++;
							}
							if( left_align )
								for( int i = 0; i < pad;
								     ++i ) {
									serial::write(
									    ' ');
									count++;
								}
						} else {
							unsigned int n =
							    k_va_arg(args, unsigned int);
							serial::write((char) n);
							unsigned int temp   = n;
							int          digits = 1;
							while( temp >= 10 ) {
								temp /= 10;
								digits++;
							}
							count += digits;
						}
						break;
					}

					default: {
						serial::write('%');
						serial::write(*p);
						count += 2;
						break;
					}
				}
			}

			k_va_end(args);
			return count;
		}
	}  // namespace debug

	/**
	 * @brief Logger type used by the logger.type() functions
	 * @param type Type of the situation. Example: error, warn, notice
	 * @param subsystem The current stage that the computer is in
	 * @param message Message to be show
	 * @param extra Extra parameters from the message
	 */
	static void logger_type(const char *subsystem,
	                        const char *type,
	                        const char *message,
	                        const char *extra) {
		if( !message || *message == '\0' )
			return;

		kstd::printf("[ ");
		kstd::printf("%.8f, ", time::get_uptime_precise());
		if( subsystem && *subsystem != '\0' )
			kstd::printf("@%s, ", subsystem);

		for( const char *t = type; *t; ++t )
			kstd::putchar((*t >= 'a' && *t <= 'z') ? (*t - 32) : *t);

		kstd::printf(" ] %s", message);

		if( extra && *extra != '\0' )
			kstd::printf(": %s", extra);

		kstd::printf("\n");
	}

	void crit(const char *s, const char *m, const char *e) {
		logger_type(s, "crit", m, e);
	}

	void alert(const char *s, const char *m, const char *e) {
		logger_type(s, "alert", m, e);
	}

	void emerg(const char *s, const char *m, const char *e) {
		logger_type(s, "emerg", m, e);
	}

	void warn(const char *s, const char *m, const char *e) {
		logger_type(s, "warn", m, e);
	}

	void error(const char *s, const char *m, const char *e) {
		logger_type(s, "error", m, e);
	}

	void notice(const char *s, const char *m, const char *e) {
		logger_type(s, "notice", m, e);
	}

	void info(const char *s, const char *m, const char *e) {
		logger_type(s, "info", m, e);
	}

	void success(const char *s, const char *m, const char *e) {
		logger_type(s, "success", m, e);
	}
}  // namespace logger

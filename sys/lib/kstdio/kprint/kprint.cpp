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
#include <kutoa.hpp>

#include <drv/tty/tty.hpp>

namespace kstd {
	void puthex(uint64_t n) {
		static const char *hex = "0123456789ABCDEF";
		char               buf[17];
		buf[16] = '\0';

		if( n == 0 ) {
			kstd::puts("0");
			return;
		}

		for( int i = 15; i >= 0; --i ) {
			buf[i] = hex[n & 0xF];
			n >>= 4;
		}

		int start = 0;
		while( start < 16 && buf[start] == '0' )
			start++;

		kstd::puts(&buf[start]);
	}

	void putdec(uint32_t n) {
		if( n == 0 ) {
			kstd::putchar('0');
			return;
		}

		char buf[11];
		int  i = 0;

		while( n > 0 ) {
			buf[i++] = (char) ('0' + (n % 10));
			n /= 10;
		}

		while( --i >= 0 ) {
			kstd::putchar(buf[i]);
		}
	}

	void puts(const char *s) {
		if( !s )
			return;
		while( *s ) {
			main_tty.write_char(*s++);
		}
	}

	// Helper function to format a double to string
	char *format_double(char *buf, size_t bufsize, double value, int precision) {
		if( bufsize < 32 )
			return buf;

		char *p = buf;

		// Handle negative
		if( value < 0.0 ) {
			*p++  = '-';
			value = -value;
		}

		// Get integer part
		uint64_t int_part  = (uint64_t) value;
		double   frac_part = value - (double) int_part;

		// Convert integer part
		char int_buf[21];
		int  idx     = 20;
		int_buf[idx] = '\0';

		if( int_part == 0 ) {
			int_buf[--idx] = '0';
		} else {
			uint64_t temp = int_part;
			while( temp > 0 ) {
				int_buf[--idx] = '0' + (temp % 10);
				temp /= 10;
			}
		}

		// Copy integer part
		while( idx < 20 ) {
			*p++ = int_buf[idx++];
		}

		// Add decimal point
		*p++ = '.';

		// Convert fractional part
		for( int i = 0; i < precision; i++ ) {
			frac_part *= 10.0;
			int digit = (int) frac_part;
			*p++      = (char) ('0' + digit);
			frac_part -= (double) digit;
		}

		*p = '\0';
		return buf;
	}

	int vsnprintf(char *buffer, size_t size, const char *format, va_list args) {
		char *out = buffer;
		char *end = buffer + (size ? size - 1 : 0);

		if( size == 0 )
			return 0;

		for( const char *p = format; *p && out < end; ++p ) {
			if( *p != '%' ) {
				*out++ = *p;
				continue;
			}
			++p;

			int left_align = 0;
			if( *p == '-' ) {
				left_align = 1;
				++p;
			}
			int width = 0;
			while( *p >= '0' && *p <= '9' ) {
				width = width * 10 + (*p++ - '0');
			}

			// Check for precision (for floats)
			int precision = 6;  // default precision
			if( *p == '.' ) {
				++p;
				precision = 0;
				while( *p >= '0' && *p <= '9' ) {
					precision = precision * 10 + (*p++ - '0');
				}
			}

			char  temp[64];
			char *t         = temp;
			int   long_long = 0;
			if( *p == 'l' && *(p + 1) == 'l' ) {
				long_long = 1;
				p += 2;
			}

			switch( *p ) {
				case 's': {
					const char *s = k_va_arg(args, const char *);
					if( !s )
						s = "(null)";
					while( *s && t < temp + sizeof(temp) - 1 ) {
						*t++ = *s++;
					}
					break;
				}
				case 'c': {
					*t++ = (char) k_va_arg(args, int);
					break;
				}
				case 'f': {
					double val = k_va_arg(args, double);
					format_double(temp, sizeof(temp), val, precision);
					t = temp;
					while( *t )
						t++;
					break;
				}
				case 'd': {
					if( long_long ) {
						int64_t  val = k_va_arg(args, int64_t);
						uint64_t uval;
						if( val < 0 ) {
							*t++ = '-';
							uval = (uint64_t) (-val);
						} else {
							uval = (uint64_t) val;
						}
						t = kstd::utoa(t,
						               temp + sizeof(temp) - 1,
						               (unsigned long) uval,
						               10,
						               0);
					} else {
						int          val = k_va_arg(args, int);
						unsigned int uval;
						if( val < 0 ) {
							*t++ = '-';
							uval = (unsigned int) (-val);
						} else {
							uval = (unsigned int) val;
						}
						t = kstd::utoa(t,
						               temp + sizeof(temp) - 1,
						               uval,
						               10,
						               0);
					}
					break;
				}
				case 'u': {
					if( long_long ) {
						uint64_t uval = k_va_arg(args, uint64_t);
						t             = kstd::utoa(t,
                                                               temp + sizeof(temp) - 1,
                                                               (unsigned long) uval,
                                                               10,
                                                               0);
					} else {
						unsigned int uval =
						    k_va_arg(args, unsigned int);
						t = kstd::utoa(t,
						               temp + sizeof(temp) - 1,
						               uval,
						               10,
						               0);
					}
					break;
				}
				case 'x': {
					unsigned int uval = k_va_arg(args, unsigned int);
					t                 = kstd::utoa(
                                            t, temp + sizeof(temp) - 1, uval, 16, 0);
					break;
				}
				case 'X': {
					unsigned int uval = k_va_arg(args, unsigned int);
					t                 = kstd::utoa(
                                            t, temp + sizeof(temp) - 1, uval, 16, 1);
					break;
				}
				case '%': {
					*t++ = '%';
					break;
				}
				default: {
					*t++ = '%';
					*t++ = *p;
					break;
				}
			}

			++p;

			size_t len = (size_t) (t - temp);
			int    pad = width > (int) len ? width - (int) len : 0;
			if( !left_align ) {
				while( pad-- > 0 && out < end )
					*out++ = ' ';
			}
			for( size_t i = 0; i < len && out < end; ++i )
				*out++ = temp[i];
			if( left_align ) {
				while( pad-- > 0 && out < end )
					*out++ = ' ';
			}
			--p;
		}

		*out = '\0';
		return (int) (out - buffer);
	}

	int snprintf(char *buffer, size_t size, const char *format, ...) {
		va_list args;
		k_va_start(args, format);
		int result = vsnprintf(buffer, size, format, args);
		k_va_end(args);
		return result;
	}

	int printf(const char *format, ...) {
		va_list args;
		k_va_start(args, format);
		int count = 0;

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
					const char *s = k_va_arg(args, const char *);
					if( !s )
						s = "(null)";
					int         len = 0;
					const char *t   = s;
					while( *t++ )
						len++;
					int pad = (width > len) ? (width - len) : 0;
					if( !left_align )
						for( int i = 0; i < pad; ++i ) {
							kstd::putchar(' ');
							count++;
						}
					for( int i = 0; i < len; ++i ) {
						kstd::putchar(s[i]);
						count++;
					}
					if( left_align )
						for( int i = 0; i < pad; ++i ) {
							kstd::putchar(' ');
							count++;
						}
					break;
				}

				case 'f': {
					double val = k_va_arg(args, double);
					char   buf[64];
					format_double(buf, sizeof(buf), val, precision);
					const char *s = buf;
					while( *s ) {
						kstd::putchar(*s++);
						count++;
					}
					break;
				}

				case 'd': {
					if( long_long ) {
						int64_t n = k_va_arg(args, int64_t);
						if( n < 0 ) {
							kstd::putchar('-');
							count++;
							n = -n;
						}
						char buf[21];
						int  idx = 20;
						buf[idx] = '\0';
						if( n == 0 )
							buf[--idx] = '0';
						else {
							uint64_t un = (uint64_t) n;
							while( un ) {
								buf[--idx] =
								    (char) ('0'
								            + (un % 10));
								un /= 10;
							}
						}
						int len = 20 - idx;
						int pad = (width > len) ? width - len : 0;
						if( !left_align )
							for( int i = 0; i < pad; ++i ) {
								kstd::putchar(' ');
								count++;
							}
						for( int i = idx; i < 20; ++i ) {
							kstd::putchar(buf[i]);
							count++;
						}
						if( left_align )
							for( int i = 0; i < pad; ++i ) {
								kstd::putchar(' ');
								count++;
							}
					} else {
						int n = k_va_arg(args, int);
						if( n < 0 ) {
							kstd::putchar('-');
							count++;
							n = -n;
						}
						// measure digits to update count
						int temp = n, digits = 1;
						while( temp >= 10 ) {
							temp /= 10;
							digits++;
						}
						putdec((uint32_t) n);
						count += digits;
					}
					break;
				}

				case 'x': {
					if( long_long ) {
						uint64_t n = k_va_arg(args, uint64_t);
						puthex(n);
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
						puthex((uint64_t) n);
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
					kstd::putchar(c);
					count++;
					break;
				}

				case '%': {
					kstd::putchar('%');
					count++;
					break;
				}

				case 'u': {
					if( long_long ) {
						uint64_t n = k_va_arg(args, uint64_t);
						char     buf[21];
						int      idx = 20;
						buf[idx]     = '\0';
						if( n == 0 )
							buf[--idx] = '0';
						else {
							while( n ) {
								buf[--idx] =
								    (char) ('0'
								            + (n % 10));
								n /= 10;
							}
						}
						int len = 20 - idx;
						int pad = (width > len) ? width - len : 0;
						if( !left_align )
							for( int i = 0; i < pad; ++i ) {
								kstd::putchar(' ');
								count++;
							}
						for( int i = idx; i < 20; ++i ) {
							kstd::putchar(buf[i]);
							count++;
						}
						if( left_align )
							for( int i = 0; i < pad; ++i ) {
								kstd::putchar(' ');
								count++;
							}
					} else {
						unsigned int n =
						    k_va_arg(args, unsigned int);
						putdec(n);
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

	void putchar(int c) {
		main_tty.write_char((char) c);
	}
}  // namespace kstd

// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/debug/debug.c
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <debug/debug.h>
#include <drivers/serial/serial.h>
#include <lib/kstdio/kstdbool.h>
#include <lib/kstdio/kstdio.h>
#include <lib/kstdlib/kutoa.h>

kbool d_enabled = ktrue;

void
    d_puts (const char *subsystem, const char *s) {
	if (!d_enabled) {
		return;
	}
	if (!s || *s == '\0')
		return;

	kputs("[    debug");

	if (subsystem && *subsystem != '\0') {
		kputs("::");
		kputs(subsystem);
	}

	kputs("] ");
	kputs(s);
	kputchar('\n');
}

int
    d_printf (const char *subsystem, const char *format, ...) {
	if (!d_enabled)
		return 0;

	if (!format || *format == '\0')
		return 1;

	va_list args;
	k_va_start(args, format);
	int count = 0;

	// Write debug header with optional subsystem
	kputs("[    debug");
	count += 10;  // Length of "[    debug"

	if (subsystem && *subsystem != '\0') {
		kputs("::");
		kputs(subsystem);
		count += 2 + (int) kstrlen(subsystem);
	}

	kputs("] ");
	count += 2;  // Length of "] "

	for (const char *p = format; *p; ++p) {
		if (*p != '%') {
			kputchar(*p);
			count++;
			continue;
		}

		p++;

		int left_align = 0;
		int width      = 0;

		if (*p == '-') {
			left_align = 1;
			p++;
		}

		while (*p >= '0' && *p <= '9') {
			width = width * 10 + (*p - '0');
			p++;
		}

		switch (*p) {
			case 's': {
				const char *s   = k_va_arg(args, const char *);
				int         len = 0;
				const char *t   = s;
				while (*t++)
					len++;

				int pad = (width > len) ? (width - len) : 0;

				if (!left_align) {
					for (int i = 0; i < pad; ++i) {
						kputchar(' ');
						count++;
					}
				}

				kputs(s);
				count += len;

				if (left_align) {
					for (int i = 0; i < pad; ++i) {
						kputchar(' ');
						count++;
					}
				}
				break;
			}

			case 'd': {
				int   n = k_va_arg(args, int);
				char  buf[12];
				char *ptr = buf;

				if (n < 0) {
					*ptr++ = '-';
					n      = -n;
				}

				char *end_ptr = kutoa(
				    ptr, buf + sizeof(buf) - 1, (unsigned int) n, 10, 0);
				*end_ptr = '\0';
				kputs(buf);
				count += (int) (end_ptr - buf);
				break;
			}

			case 'x': {
				unsigned int n = k_va_arg(args, unsigned int);
				char         buf[12];
				char        *end_ptr =
				    kutoa(buf, buf + sizeof(buf) - 1, n, 16, 0);
				*end_ptr = '\0';
				kputs(buf);
				count += (int) (end_ptr - buf);
				break;
			}
			case 'l': {
				// Handle long/long long modifiers
				if (*(p + 1) == 'l') {
					p++;  // Skip second 'l'
					if (*(p + 1) == 'x') {
						p++;  // Skip 'x'
						kuint64_t n = k_va_arg(args, kuint64_t);
						char      buf[20];
						char     *end_ptr =
						    kutoa(buf,
						          buf + sizeof(buf) - 1,
						          (unsigned int) (n & 0xFFFFFFFF),
						          16,
						          0);
						*end_ptr = '\0';
						kputs(buf);
						count += (int) (end_ptr - buf);
						break;
					}
				}
				// Fall through to default for single 'l'
				goto default_case;
			}

			case 'c': {
				char c = (char) k_va_arg(args, int);
				kputchar(c);
				count++;
				break;
			}

			case '%': {
				kputchar('%');
				count++;
				break;
			}

			default:
			default_case: {
				kputchar('%');
				kputchar(*p);
				count += 2;
				break;
			}
		}
	}

	k_va_end(args);
	return count;
}

static void
    d_dbgtype (const char *type,
               const char *subsystem,
               const char *message,
               const char *extra) {
	if (!message || *message == '\0')
		return;

	kprintf("[    %s", type);

	if (subsystem && *subsystem != '\0')
		kprintf("::%s", subsystem);

	kprintf("] %s", message);

	if (extra && *extra != '\0')
		kprintf(": %s", extra);

	kprintf("\n");
}

static void
    d_crit (const char *s, const char *m, const char *e) {
	d_dbgtype("crit", s, m, e);
}

static void
    d_alert (const char *s, const char *m, const char *e) {
	d_dbgtype("alert", s, m, e);
}

static void
    d_emerg (const char *s, const char *m, const char *e) {
	d_dbgtype("emerg", s, m, e);
}

static void
    d_warn (const char *s, const char *m, const char *e) {
	d_dbgtype("warn", s, m, e);
}

static void
    d_err (const char *s, const char *m, const char *e) {
	d_dbgtype("err", s, m, e);
}

static void
    d_notice (const char *s, const char *m, const char *e) {
	d_dbgtype("notice", s, m, e);
}

static void
    d_info (const char *s, const char *m, const char *e) {
	d_dbgtype("info", s, m, e);
}

struct Debug debug = {.crit   = d_crit,
                      .alert  = d_alert,
                      .emerg  = d_emerg,
                      .warn   = d_warn,
                      .err    = d_err,
                      .notice = d_notice,
                      .info   = d_info,
                      .puts   = d_puts,
                      .printf = d_printf};

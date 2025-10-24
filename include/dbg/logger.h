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

namespace logger {
	void puts(const char *subsystem, const char *type, const char *message);
	int  printf(const char *subsystem, const char *type, const char *format, ...);
	void crit(const char *s, const char *m, const char *e);
	void alert(const char *s, const char *m, const char *e);
	void emerg(const char *s, const char *m, const char *e);
	void warn(const char *s, const char *m, const char *e);
	void error(const char *s, const char *m, const char *e);
	void notice(const char *s, const char *m, const char *e);
	void info(const char *s, const char *m, const char *e);
	void success(const char *s, const char *m, const char *e);
}  // namespace logger

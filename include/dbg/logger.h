// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : include/dbg/logger.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

extern struct Logger logger;

struct Logger {
	void (*crit)(const char *, const char *, const char *);
	void (*alert)(const char *, const char *, const char *);
	void (*emerg)(const char *, const char *, const char *);
	void (*warn)(const char *, const char *, const char *);
	void (*err)(const char *, const char *, const char *);
	void (*notice)(const char *, const char *, const char *);
	void (*info)(const char *, const char *, const char *);
	void (*success)(const char *s, const char *m, const char *e);
	void (*puts)(const char *subsystem, const char *type, const char *s);
	int (*printf)(const char *subsystem, const char *type, const char *format, ...);
};

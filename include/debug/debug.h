// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        include/debug/debug.h
 * Author(s):   Tempik25 <tempik25@tempestfoundation.org>
 *              (https://github.com/tempik25)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#pragma once

extern struct Debug debug;

struct Debug {
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

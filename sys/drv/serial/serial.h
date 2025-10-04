// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/drv/serial/serial.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <kstdbool.h>

extern struct Serial serial;

struct Serial {
	void (*init)(void);
	void (*write)(char a);
	void (*writes)(const char *s);
	void (*write_int)(int i);
	kbool (*is_available)(void);
};

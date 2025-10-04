// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/fs/vfs/vfs.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <kstddef.h>

extern struct Vfs vfs;

struct Vfs {
	void (*getcwd)(char *out, ksize_t size);
	void (*normalize)(const char *path, char *out, ksize_t size);
	void (*resolve)(const char *path, char *out, ksize_t size);
	int (*chdir)(const char *path);
};

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
#include <kprint.h>

#include <fs/vfs/vfs.h>

void
    cmd_cd(const char *args) {
	const char *path = (args && *args) ? args : "/";
	int         rc   = vfs::chdir(path);
	if( rc != 0 ) {
		kstd::printf("cd: cannot access %s (err %d)\n", path, rc);
	}
}

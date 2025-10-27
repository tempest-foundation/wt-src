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
#include "ls.h"

#include <kstdint.h>

#include "helper.h"

#include <fs/vfs/vfs.h>

void cmd_ls(const char *args) {
	// If no path given use current working directory
	const char *path = (args && *args) ? args : nullptr;
	char        buf[256];
	if( !path ) {
		vfs::getcwd(buf, sizeof(buf));
		path = buf;
	} else if( path[0] != '/' ) {
		vfs::resolve(path, buf, sizeof(buf));
		path = buf;
	}
	list_dir_path(path);
}

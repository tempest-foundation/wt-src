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

#include <fs/ext2/ext2.h>
#include <fs/vfs/vfs.h>

void
    cmd_cat(const char *args) {
	if( !args || *args == '\0' ) {
		kstd::puts("Usage: cat <path>");
		return;
	}
	char        abs_path[256];
	const char *path = args;
	if( path[0] != '/' ) {
		vfs::resolve(path, abs_path, sizeof(abs_path));
		path = abs_path;
	}
	ext2_file_t file;
	int         rc = ext2::open(path, &file);
	if( rc != 0 ) {
		kstd::printf("cat: cannot open %s (err %d)\n", path, rc);
		return;
	}
	char buf[512];
	int  read;
	while( (read = ext2::read(&file, buf, sizeof(buf) - 1)) > 0 ) {
		buf[read] = '\0';
		kstd::puts(buf);
	}
}

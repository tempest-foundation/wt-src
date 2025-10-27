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
#include "fsize.hpp"

#include <kprint.hpp>

#include <fs/ext2/ext2.hpp>
#include <fs/vfs/vfs.hpp>

void cmd_fsize(const char *args) {
	if( !args || *args == '\0' ) {
		kstd::puts("Usage: fsize <path>");
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
		kstd::printf("fsize: cannot open %s (err %d)\n", path, rc);
		return;
	}
	uint64_t size =
	    (((uint64_t) file.inode.dir_acl_or_size_high) << 32) | file.inode.size_lo;
	kstd::printf("%s: %llu bytes\n", args, size);
}

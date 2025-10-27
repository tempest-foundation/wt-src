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

void ls_print_cb(const char *name, uint8_t file_type) {
	(void) file_type;
	kstd::puts(name);
	kstd::puts("\n");
}

void list_dir_path(const char *path) {
	int rc = ext2::list(path, ls_print_cb);
	if( rc != 0 ) {
		kstd::printf("ls: cannot access %s (err %d)\n", path, rc);
	}
}

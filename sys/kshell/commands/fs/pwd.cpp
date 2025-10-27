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

void cmd_pwd(const char *args) {
	(void) args;
	char buf[256];
	vfs::getcwd(buf, sizeof(buf));
	kstd::puts(buf);
}

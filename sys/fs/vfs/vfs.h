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
#pragma once

#include <kstddef.h>

#include "vfs_fs_ops.h"

namespace vfs {
	void                 register_fs(vfs_fs_operations_t *fs_ops);
	void                 set_root_fs(vfs_fs_operations_t *fs_ops);
	vfs_fs_operations_t *get_root_fs(void);
	void                 getcwd(char *out, size_t size);
	void                 normalize_path(const char *path, char *out, size_t size);
	void                 resolve(const char *path, char *out, size_t size);
	int                  chdir(const char *path);
}  // namespace vfs

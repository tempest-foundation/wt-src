// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/fs/ext2/vfs_adapter.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <fs/ext2/ext2.h>
#include <fs/vfs/vfs_fs_ops.h>
#include <kern/memory/memory.h>

static vfs_fs_operations_t ext2_vfs_ops;

/**
 * @brief Opens a file using ext2 filesystem
 */
static int
    ext2_vfs_open (const char *path, vfs_file_t *file) {
	ext2_file_t *ext2_file = kmalloc(sizeof(ext2_file_t));
	if (!ext2_file)
		return -1;

	int rc = ext2.open(path, ext2_file);
	if (rc != 0) {
		kfree(ext2_file);
		return rc;
	}

	file->fs_specific = ext2_file;
	file->mode        = ext2_file->inode.mode;
	file->fs_ops      = &ext2_vfs_ops;

	return 0;
}

/**
 * @brief Closes an ext2 file
 */
static int
    ext2_vfs_close (vfs_file_t *file) {
	if (file && file->fs_specific) {
		// If ext2 has a close function, call it here
		// ext2.close((ext2_file_t *)file->fs_specific);
		kfree(file->fs_specific);
		file->fs_specific = KNULL;
	}
	return 0;
}

/**
 * @brief Checks if the file is a directory
 */
static int
    ext2_vfs_is_directory (vfs_file_t *file) {
	if (!file)
		return 0;
	// Check for directory bit (0x4000) in ext2 mode
	return (file->mode & 0x4000) != 0;
}

/**
 * @brief EXT2 filesystem operations for VFS
 */
static vfs_fs_operations_t ext2_vfs_ops = {
    .open         = ext2_vfs_open,
    .close        = ext2_vfs_close,
    .is_directory = ext2_vfs_is_directory,
    .name         = "ext2",
};

/**
 * @brief Initializes ext2 with VFS
 *
 * Call this function during system initialization to register
 * ext2 as the root filesystem.
 */
void
    ext2_vfs_init (void) {
	vfs_register_fs(&ext2_vfs_ops);
	vfs_set_root_fs(&ext2_vfs_ops);
}

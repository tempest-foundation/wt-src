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

#include <fs/ext2/ext2.hpp>
#include <fs/vfs/vfs_fs_ops.hpp>
#include <kern/memory/memory.hpp>

namespace ext2 {
	static int vfs_open(const char *path, vfs_file_t *file);
	static int vfs_close(vfs_file_t *file);
	static int vfs_is_directory(vfs_file_t *file);

	/**
	 * @brief EXT2 filesystem operations for VFS
	 */
	static vfs_fs_operations_t ext2_vfs_ops = {
	    .open         = vfs_open,
	    .close        = vfs_close,
	    .is_directory = vfs_is_directory,
	    .name         = "ext2",
	};

	/**
	 * @brief Opens a file using ext2 filesystem
	 */
	static int vfs_open(const char *path, vfs_file_t *file) {
		auto *ext2_file = (ext2_file_t *) memory::malloc(sizeof(ext2_file_t));
		if( !ext2_file )
			return -1;

		int rc = ext2::open(path, ext2_file);
		if( rc != 0 ) {
			memory::free(ext2_file);
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
	static int vfs_close(vfs_file_t *file) {
		if( file && file->fs_specific ) {
			memory::free(file->fs_specific);
			file->fs_specific = nullptr;
		}
		return 0;
	}

	/**
	 * @brief Checks if the file is a directory
	 */
	static int vfs_is_directory(vfs_file_t *file) {
		if( !file )
			return 0;
		return (file->mode & 0x4000) != 0;
	}

	/**
	 * @brief Initializes ext2 with VFS
	 */
	void vfs_init(void) {
		vfs_register_fs(&ext2_vfs_ops);
		vfs_set_root_fs(&ext2_vfs_ops);
	}
}  // namespace ext2

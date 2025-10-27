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

#include <kstdint.hpp>

/**
 * @brief Generic file descriptor structure for VFS
 */
typedef struct {
	void    *fs_specific;  // Filesystem-specific file structure
	uint32_t mode;         // File mode/type
	void    *fs_ops;       // Pointer back to filesystem operations
} vfs_file_t;

/**
 * @brief Filesystem operations structure
 *
 * Provides an abstraction layer for different filesystem implementations.
 * Each filesystem (ext2, fat32, etc.) should implement these operations.
 */
typedef struct vfs_fs_operations {
	/**
	 * @brief Opens a file or directory
	 * @param path Absolute path to the file/directory
	 * @param file Output parameter for file descriptor
	 * @return 0 on success, negative error code on failure
	 */
	int (*open)(const char *path, vfs_file_t *file);

	/**
	 * @brief Closes a file or directory
	 * @param file File descriptor to close
	 * @return 0 on success, negative error code on failure
	 */
	int (*close)(vfs_file_t *file);

	/**
	 * @brief Checks if a file descriptor represents a directory
	 * @param file File descriptor to check
	 * @return 1 if directory, 0 if not
	 */
	int (*is_directory)(vfs_file_t *file);

	/**
	 * @brief Filesystem name (e.g., "ext2", "fat32")
	 */
	const char *name;
} vfs_fs_operations_t;

/**
 * @brief Registers a filesystem driver with the VFS
 * @param fs_ops Pointer to filesystem operations structure
 */
void vfs_register_fs(vfs_fs_operations_t *fs_ops);

/**
 * @brief Sets the active root filesystem
 * @param fs_ops Pointer to filesystem operations structure
 */
void vfs_set_root_fs(vfs_fs_operations_t *fs_ops);

/**
 * @brief Gets the current root filesystem operations
 * @return Pointer to current root filesystem operations, or NULL if none set
 */
vfs_fs_operations_t *vfs_get_root_fs(void);

// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/fs/vfs/vfs.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "vfs.h"

#include <kstdio.h>
#include <kstring.h>

#include "vfs_fs_ops.h"

#include <fs/ext2/ext2.h>

static char                 cwd_path[256] = "/";
static vfs_fs_operations_t *root_fs       = KNULL;

/**
 * @brief Registers a filesystem driver with the VFS
 */
void
    vfs_register_fs (vfs_fs_operations_t *fs_ops) {
	// TODO: Could maintain a list of registered filesystems here
	// For now, we just use root_fs
	(void) fs_ops;
}

/**
 * @brief Sets the active root filesystem
 */
void
    vfs_set_root_fs (vfs_fs_operations_t *fs_ops) {
	root_fs = fs_ops;
}

/**
 * @brief Gets the current root filesystem operations
 */
vfs_fs_operations_t *
    vfs_get_root_fs (void) {
	return root_fs;
}

/**
 * @brief Retrieves the current working directory path.
 *
 * Copies the current working directory path into the provided output buffer.
 *
 * @param out   Pointer to the output buffer where the path will be stored.
 * @param size  Size of the output buffer.
 */
void
    vfs_getcwd (char *out, ksize_t size) {
	kstrncpy(out, cwd_path, size);
}

/**
 * @brief Normalizes an absolute filesystem path, resolving ".." and "." components.
 *
 * Given an input path, produces an absolute, clean, and normalized path.
 *
 * Normalization removes redundant slashes, resolves "." and ".." path components,
 * and collapses all path elements to their canonical representation.
 *
 * Examples:
 *   "/a/b/./c/../d/" => "/a/b/d"
 *   "/../"           => "/"
 *   "/a//b/c"        => "/a/b/c"
 *
 * - If the input path is empty or NULL, "/" is output.
 * - If the input path is not absolute (doesn't start with '/'), it is copied as-is.
 *
 * @param path  The input path to normalize.  Should be an absolute path starting with '/'.
 * @param out   Pointer to output buffer which will contain the normalized path on return.
 * @param size  Size of the output buffer.
 * @warning Paths longer than 255 characters may be truncated or cause undefined behavior.
 *          because of the internal temporary buffer size.
 */
void
    vfs_normalize_path (const char *path, char *out, ksize_t size) {
	if (!path || *path == '\0') {
		kstrncpy(out, "/", size);
		return;
	}

	// Require absolute path
	if (path[0] != '/') {
		kstrncpy(out, path, size);
		return;
	}

	char    tmp[256] = {0};
	ksize_t pos      = 0;
	tmp[pos++]       = '/';

	const char *p = path + 1;  // skip leading '/'
	while (*p) {
		// Skip repeated '/'
		while (*p == '/')
			++p;
		if (!*p)
			break;

		const char *start = p;
		while (*p && *p != '/')
			++p;
		ksize_t len = (ksize_t) (p - start);

		char comp[256];
		kmemcpy(comp, start, len);
		comp[len] = '\0';

		if (len == 1 && comp[0] == '.')
			continue;  // ignore '.'
		if (len == 2 && comp[0] == '.' && comp[1] == '.') {
			// Go up one directory
			if (pos > 1) {
				// remove trailing '/'
				if (tmp[pos - 1] == '/')
					--pos;
				// pop until previous '/'
				while (pos > 0 && tmp[pos - 1] != '/')
					--pos;
			}
			continue;
		}

		if (pos > 1 && tmp[pos - 1] != '/')
			tmp[pos++] = '/';
		if (pos + len >= sizeof(tmp))
			break;  // prevent overflow
		kmemcpy(&tmp[pos], comp, len);
		pos += len;
	}

	if (pos > 1 && tmp[pos - 1] == '/')
		--pos;
	tmp[pos] = '\0';

	kstrncpy(out, tmp, size);
}

/**
 * @brief Resolves a given path to an absolute, normalized path.
 *
 * Takes a user-supplied path (which can be absolute or relative)
 * and produces an absolute, normalized path string in @p out.  It combines the path with
 * the current working directory if needed, and normalizes directory components.
 *
 * @param path  The input path to resolve.  Can be relative or absolute.  If NULL or empty,
 *              the current working directory is used.
 * @param out   Pointer to the output buffer where the resolved path will be stored.
 * @param size  Size of the output buffer.
 */
void
    vfs_resolve (const char *path, char *out, ksize_t size) {
	char temp[256] = {0};
	if (!path || *path == '\0') {
		kstrcpy(temp, cwd_path);
	} else if (path[0] == '/') {
		// Absolute
		kstrcpy(temp, path);
	} else {
		if (kstrcmp(cwd_path, "/") == 0)
			ksnprintf(temp, sizeof(temp), "/%s", path);
		else
			ksnprintf(temp, sizeof(temp), "%s/%s", cwd_path, path);
	}
	vfs_normalize_path(temp, out, size);
}

/**
 * @brief Changes the current working directory.
 *
 * Resolves the provided @p path to an absolute, normalized path, attempts to open it
 * as a directory using the underlying filesystem, and if successful,
 * sets the process's current working directory to the resolved path.
 *
 * @param path Path to the directory to change to.  Can be relative or absolute.
 * @return 0 on success, or a negative error code on failure.
 *
 *         Returns an error if:
 *           - No root filesystem is registered.
 *           - The directory does not exist.
 *           - The path does not refer to a directory.
 *           - A filesystem-specific error occurs.
 */
int
    vfs_chdir (const char *path) {
	if (!root_fs || !root_fs->open || !root_fs->is_directory)
		return -1;  // No filesystem registered

	char resolved[256] = {0};
	vfs_resolve(path, resolved, sizeof(resolved));

	vfs_file_t file = {0};
	int        rc   = root_fs->open(resolved, &file);
	if (rc != 0)
		return rc;  // Propagate filesystem error

	if (!root_fs->is_directory(&file)) {
		if (root_fs->close)
			root_fs->close(&file);
		return -1;  // Not a directory
	}

	if (root_fs->close)
		root_fs->close(&file);

	kstrcpy(cwd_path, resolved);
	return 0;
}

struct Vfs vfs = {
    .getcwd    = vfs_getcwd,
    .normalize = vfs_normalize_path,
    .resolve   = vfs_resolve,
    .chdir     = vfs_chdir,
};

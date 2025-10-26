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
#include "ext2.h"

#include <kstdio.h>
#include <kstring.h>

#include <kern/memory/memory.h>

static uint64_t           g_base_lba = 0;  // Partition/LBA base of filesystem
static ext2_superblock_t  g_superblock;
static ext2_group_desc_t *g_group_desc = nullptr;
static uint32_t           g_block_size = 0;
bool                      is_mounted   = false;

// Caller-supplied I/O functions (sector based, 512-byte)
static int (*g_read_sector)(uint64_t lba, uint32_t count, void *buf)        = nullptr;
static int (*g_write_sector)(uint64_t lba, uint32_t count, const void *buf) = nullptr;

/**
 * @brief Read multiple 512-byte sectors from the block device into memory.
 *
 * This is a convenience wrapper around the block device read callback.
 * Reads @p cnt sectors, starting at LBA @p lba, into the buffer @p dst.
 *
 * @param[in]  lba  The starting logical block address (LBA) to read from (sector-sized, 512 bytes).
 * @param[in]  cnt  Number of sectors to read.
 * @param[out] dst  Pointer to the memory buffer to store read data (at least cnt*512 bytes).
 *
 * @retval 0        Success.
 * @retval <0       EXT2_ERR_IO on error or if no read callback is installed.
 */
static int
    kread_sectors(uint64_t lba, uint32_t cnt, void *dst) {
	if( !g_read_sector ) {
		return EXT2_ERR_IO;
	}
	return g_read_sector(lba, cnt, dst);
}

/**
 * @brief Read a full filesystem block from disk into memory.
 *
 * Reads the specified EXT2 filesystem block (by @p block_id) into the buffer @p dst.
 * The size of each block is given by g_block_size (1KiB, 2KiB, or 4KiB).
 * The @p dst buffer must be at least g_block_size bytes.
 *
 * @param[in]  block_id  Filesystem block index (not LBA/sector) relative
 *                        to the volume start.
 * @param[out] dst       Pointer to memory buffer to store the block contents
 *                       (at least g_block_size bytes).
 *
 * @retval 0        Success.
 * @retval <0       EXT2_ERR_IO on error.
 */
static int
    kread_block(uint32_t block_id, void *dst) {
	uint64_t first_lba = g_base_lba + ((uint64_t) block_id * g_block_size) / 512;
	uint32_t cnt       = g_block_size / 512;
	return kread_sectors(first_lba, cnt, dst);
}

/**
 * @brief Read an inode structure from disk into memory.
 *
 * Given an EXT2 inode number @p ino, reads the corresponding inode structure from disk
 * and copies it into the user-supplied buffer @p out.  The function calculates which block
 * group and index within the group the inode resides in, locates the proper block(s) in
 * the inode table, and performs the necessary I/O to read the inode data.
 *
 * This function handles cases where the inode structure spans two filesystem blocks and
 * reads both blocks if necessary.  The inode is read as an ext2_inode_t (typically 128 bytes,
 * but can be larger depending on EXT2_SUPERBLOCK.inode_size).
 *
 * @param[in]  ino  The inode number to read (must be nonzero; 1 = first inode).
 * @param[out] out  Pointer to a buffer where the inode structure will be stored.  Must point to
 *                  memory at least sizeof(ext2_inode_t) bytes long.
 *
 * @retval EXT2_OK          Success; inode structure has been copied to @p out.
 * @retval EXT2_ERR_INVALID Invalid arguments (ino==0, missing group descriptor, etc).
 * @retval EXT2_ERR_IO      I/O error (e.g., failed block read, allocation error).
 */
static int
    kread_inode(uint32_t ino, ext2_inode_t *out) {
	if( ino == 0 )
		return EXT2_ERR_INVALID;

	uint32_t idx0  = ino - 1;
	uint32_t group = idx0 / g_superblock.inodes_per_group;
	uint32_t index = idx0 % g_superblock.inodes_per_group;

	if( !g_group_desc )
		return EXT2_ERR_INVALID;

	uint32_t inode_tbl_blk = g_group_desc[group].inode_table;
	uint32_t inode_size    = g_superblock.inode_size ? g_superblock.inode_size : 128;

	uint64_t offset       = (uint64_t) index * inode_size;
	uint32_t blk_offset   = (uint32_t) (offset / g_block_size);
	uint32_t off_in_block = (uint32_t) (offset % g_block_size);

	uint8_t *tmp = (uint8_t *) memory::malloc(g_block_size);
	if( !tmp )
		return EXT2_ERR_IO;

	// Read the first block that contains (part of) the inode
	if( kread_block(inode_tbl_blk + blk_offset, tmp) != 0 ) {
		memory::free(tmp);
		return EXT2_ERR_IO;
	}

	uint32_t bytes_first = g_block_size - off_in_block;
	uint32_t need        = sizeof(ext2_inode_t);

	if( bytes_first >= need ) {
		// Inode fits entirely in this block
		kstring::memcpy(out, tmp + off_in_block, need);
	} else {
		// Copy first portion
		kstring::memcpy(out, tmp + off_in_block, bytes_first);

		// Read next block for the remaining bytes
		if( kread_block(inode_tbl_blk + blk_offset + 1, tmp) != 0 ) {
			memory::free(tmp);
			return EXT2_ERR_IO;
		}
		kstring::memcpy((uint8_t *) out + bytes_first, tmp, need - bytes_first);
	}

	memory::free(tmp);
	return EXT2_OK;
}

/**
 * @brief Processes a directory block and invokes a callback for each valid directory entry.
 *
 * Iterates over all directory entries within a given raw EXT2 directory block, calling the
 * user-supplied callback @p cb for each valid entry found.
 * Handles proper parsing of directory entry records and safeguards against malformed data.
 *
 * @param[in] block_buf   Pointer to the buffer containing the raw directory block data.
 *                        Must be at least @p block_size bytes.
 * @param[in] block_size  Size of the directory block in bytes.
 * @param[in] cb          Callback function to be invoked for each valid directory entry.
 *                        The callback receives the entry name (NUL-terminated) and file type.
 *
 * The function extracts the name and file type from each entry and passes them to the callback.
 * Invalid or malformed entries (incorrect record length, zero-length names, etc.) will cause
 * processing of the block to end early.
 */
static void
    kprocess_dir_block(uint8_t *block_buf, uint32_t block_size, ext2_list_cb_t cb) {
	uint32_t off = 0;
	while( off < block_size ) {
		ext2_dir_entry_t *ent     = (ext2_dir_entry_t *) (block_buf + off);
		uint16_t          rec_len = ent->rec_len;
		uint8_t           nlen    = ent->name_len;
		if( rec_len < 8 || rec_len > block_size - off )
			break;
		if( ent->inode != 0 && nlen == 0 )
			break;
		uint16_t min_len = (uint16_t) (((8 + nlen + 3) / 4) * 4);
		if( rec_len < min_len )
			break;
		if( ent->inode != 0 && nlen > 0 ) {
			char name[256] = {0};
			kstring::memcpy(name, ent->name, nlen);
			name[nlen] = '\0';
			cb(name, ent->file_type);
		}
		off += rec_len;
	}
}

/**
 * @brief List the entries in a directory given its inode on the EXT2 filesystem.
 *
 * This function enumerates all directory entries in the direct blocks (the first 12 data blocks)
 * of the specified directory inode, invoking the callback function @p cb for each entry found.
 * Indirect, double indirect, or triple indirect directory blocks are not processed.
 *
 * @param[in] dir_inode Pointer to the EXT2 inode structure representing a directory (must not be NULL).
 * @param[in] cb        Callback function to invoke for each directory entry (must not be NULL).
 *
 * @return EXT2_OK on success, or a negative EXT2_ERR_* code on error:
 *         - EXT2_ERR_INVALID : Invalid arguments or inode is not a directory.
 *         - EXT2_ERR_IO      : I/O error or out-of-memory.
 */
static int
    klist_dir_entries(const ext2_inode_t *dir_inode, ext2_list_cb_t cb) {
	if( !dir_inode || !cb )
		return EXT2_ERR_INVALID;

	if( !(dir_inode->mode & 0x4000) )  // not a directory
		return EXT2_ERR_INVALID;

	uint8_t *block_buf = (uint8_t *) memory::malloc(g_block_size);
	if( !block_buf )
		return EXT2_ERR_IO;

	for( int i = 0; i < 12; i++ ) {
		uint32_t blk_id = dir_inode->block[i];
		if( blk_id == 0 )
			continue;
		if( kread_block(blk_id, block_buf) != 0 ) {
			memory::free(block_buf);
			return EXT2_ERR_IO;
		}
		kprocess_dir_block(block_buf, g_block_size, cb);
	}
	memory::free(block_buf);
	return EXT2_OK;
}

/**
 * @brief Retrieve the physical block number for a given logical block in a file.
 *
 * Given an EXT2 inode and a logical block number (relative to the start of the file), this function
 * returns the physical block number on disk that stores the data for that logical block.  It handles
 * direct blocks, single indirect, double indirect, and triple indirect block addressing as per the
 * EXT2 specification.  If the block pointer is not allocated (sparse file), this function returns 0.
 *
 * @param[in] inode         Pointer to the EXT2 inode structure of a file (must not be NULL).
 * @param[in] logical_block The logical block index within the file (0 = first block).
 *
 * @return The physical block number for the given logical block, or 0 if the block is sparse/unallocated,
 *         or if an I/O error occurs, or if the block index is outside the supported address range.
 */
static uint32_t
    kget_file_block(const ext2_inode_t *inode, uint64_t logical_block) {
	uint32_t blocks_per_indirect = g_block_size / sizeof(uint32_t);

	// Direct blocks (0-11)
	if( logical_block < 12 ) {
		return inode->block[logical_block];
	}

	// Single indirect blocks (12 to 12 + blocks_per_indirect - 1)
	uint64_t single_indirect_start = 12;
	uint64_t single_indirect_end   = single_indirect_start + blocks_per_indirect;

	if( logical_block < single_indirect_end ) {
		uint32_t indirect_block = inode->block[12];
		if( indirect_block == 0 )
			return 0;  // Sparse

		// Read the indirect block
		uint32_t *indirect_buf = (uint32_t *) memory::malloc(g_block_size);
		if( !indirect_buf )
			return 0;

		if( kread_block(indirect_block, indirect_buf) != 0 ) {
			memory::free(indirect_buf);
			return 0;
		}

		uint32_t index  = (uint32_t) (logical_block - single_indirect_start);
		uint32_t result = indirect_buf[index];
		memory::free(indirect_buf);
		return result;
	}

	// Double indirect blocks
	uint64_t double_indirect_start = single_indirect_end;
	uint64_t double_indirect_end =
	    double_indirect_start + (uint64_t) blocks_per_indirect * blocks_per_indirect;

	if( logical_block < double_indirect_end ) {
		uint32_t double_indirect_block = inode->block[13];
		if( double_indirect_block == 0 )
			return 0;  // Sparse

		// Calculate which single indirect block we need
		uint64_t offset_in_double = logical_block - double_indirect_start;
		uint32_t single_indirect_index =
		    (uint32_t) (offset_in_double / blocks_per_indirect);
		uint32_t data_block_index =
		    (uint32_t) (offset_in_double % blocks_per_indirect);

		// Read the double indirect block to get the single indirect block pointer
		uint32_t *double_indirect_buf = (uint32_t *) memory::malloc(g_block_size);
		if( !double_indirect_buf )
			return 0;

		if( kread_block(double_indirect_block, double_indirect_buf) != 0 ) {
			memory::free(double_indirect_buf);
			return 0;
		}

		uint32_t single_indirect_block =
		    double_indirect_buf[single_indirect_index];
		memory::free(double_indirect_buf);

		if( single_indirect_block == 0 )
			return 0;  // Sparse

		// Now read the single indirect block to get the data block pointer
		uint32_t *single_indirect_buf = (uint32_t *) memory::malloc(g_block_size);
		if( !single_indirect_buf )
			return 0;

		if( kread_block(single_indirect_block, single_indirect_buf) != 0 ) {
			memory::free(single_indirect_buf);
			return 0;
		}

		uint32_t result = single_indirect_buf[data_block_index];
		memory::free(single_indirect_buf);
		return result;
	}

	// Triple indirect blocks
	uint64_t triple_indirect_start = double_indirect_end;
	uint64_t triple_indirect_end =
	    triple_indirect_start
	    + (uint64_t) blocks_per_indirect * blocks_per_indirect * blocks_per_indirect;

	if( logical_block < triple_indirect_end ) {
		uint32_t triple_indirect_block = inode->block[14];
		if( triple_indirect_block == 0 )
			return 0;  // Sparse

		// Calculate indices for triple indirection
		uint64_t offset_in_triple = logical_block - triple_indirect_start;
		uint32_t double_indirect_index =
		    (uint32_t) (offset_in_triple
		                / (blocks_per_indirect * blocks_per_indirect));
		uint32_t single_indirect_index =
		    (uint32_t) ((offset_in_triple
		                 % (blocks_per_indirect * blocks_per_indirect))
		                / blocks_per_indirect);
		uint32_t data_block_index =
		    (uint32_t) (offset_in_triple % blocks_per_indirect);

		// Read the triple indirect block to get the double indirect block pointer
		uint32_t *triple_indirect_buf = (uint32_t *) memory::malloc(g_block_size);
		if( !triple_indirect_buf )
			return 0;

		if( kread_block(triple_indirect_block, triple_indirect_buf) != 0 ) {
			memory::free(triple_indirect_buf);
			return 0;
		}

		uint32_t double_indirect_block =
		    triple_indirect_buf[double_indirect_index];
		memory::free(triple_indirect_buf);

		if( double_indirect_block == 0 )
			return 0;  // Sparse

		// Read the double indirect block to get the single indirect block pointer
		uint32_t *double_indirect_buf = (uint32_t *) memory::malloc(g_block_size);
		if( !double_indirect_buf )
			return 0;

		if( kread_block(double_indirect_block, double_indirect_buf) != 0 ) {
			memory::free(double_indirect_buf);
			return 0;
		}

		uint32_t single_indirect_block =
		    double_indirect_buf[single_indirect_index];
		memory::free(double_indirect_buf);

		if( single_indirect_block == 0 )
			return 0;  // Sparse

		// Finally read the single indirect block to get the data block pointer
		uint32_t *single_indirect_buf = (uint32_t *) memory::malloc(g_block_size);
		if( !single_indirect_buf )
			return 0;

		if( kread_block(single_indirect_block, single_indirect_buf) != 0 ) {
			memory::free(single_indirect_buf);
			return 0;
		}

		uint32_t result = single_indirect_buf[data_block_index];
		memory::free(single_indirect_buf);
		return result;
	}

	// Fuck you if you want more than triple indirect blocks.
	return 0;
}

// Forward declaration for functions used before definition
static int
    kread_inode(uint32_t ino, ext2_inode_t *out);

namespace ext2 {
	/**
 * @brief Sets the low-level block device I/O functions for EXT2 filesystem operations.
 *
 * This function installs the block device read and write callbacks used by the EXT2 driver to
 * perform sector-based I/O operations.  These functions must match the required prototypes:
 *   - Read:  int read(uint64_t lba, uint32_t count, void *buf)
 *   - Write: int write(uint64_t lba, uint32_t count, const void *buf)
 *
 * The write function may be NULL (nullptr) if the filesystem is to be accessed as read-only.
 *
 * @param[in] read   Pointer to a function that reads sectors from the block device.
 * @param[in] write  Pointer to a function that writes sectors to the block device (may be NULL).
 *
 * @note This function must be called before mounting an EXT2 filesystem.
 */
	void set_block_device(int (*read)(uint64_t, uint32_t, void *),
	                      int (*write)(uint64_t, uint32_t, const void *)) {
		g_read_sector  = read;
		g_write_sector = write;  // May be nullptr for read-only volumes
	}

	/**
 * @brief Mounts an EXT2 filesystem at the specified base LBA.
 *
 * This function initializes the EXT2 filesystem structures by reading
 * the superblock and group descriptor table from disk, checking the
 * EXT2 magic, and validating supported block size.  It must be called
 * before using any other EXT2 operations.  The base LBA should point to
 * the beginning of the EXT2 partition or volume.
 *
 * This function supports EXT2 filesystems with 1K, 2K, or 4K block sizes.
 * Non-standard block sizes larger than 4K are rejected.
 *
 * @param[in] base_lba The LBA (sector) offset where the filesystem starts.
 *
 * @retval EXT2_OK              Success; the filesystem is mounted.
 * @retval EXT2_ERR_IO          Low-level read error or out-of-memory.
 * @retval EXT2_ERR_BAD_MAGIC   The filesystem magic field is incorrect (not EXT2).
 * @retval EXT2_ERR_UNSUPPORTED The filesystem uses an unsupported block size.
 */
	int mount(uint64_t base_lba) {
		if( !g_read_sector ) {
			return EXT2_ERR_IO;
		}

		g_base_lba = base_lba;

		/*
	 * Read raw 1024-byte superblock into temp buffer and copy only the
	 * structure portion we need. This avoids overflowing g_superblock
	 * (struct is ~128 bytes).
	 */
		uint8_t sb_raw[1024];
		if( kread_sectors(base_lba + EXT2_SUPERBLOCK_OFFSET / 512, 2, sb_raw)
		    != 0 ) {
			return EXT2_ERR_IO;
		}
		kstring::memcpy(&g_superblock, sb_raw, sizeof(ext2_superblock_t));

		if( g_superblock.magic != EXT2_SUPER_MAGIC )
			return EXT2_ERR_BAD_MAGIC;

		/*
	 * Compute block size and validate (EXT2 supports 1K,2K,4K). Anything
	 * larger is non-standard and can easily blow up our simple memory::malloc().
	 * Reject filesystems with block sizes >4 KiB.
	 */
		if( g_superblock.log_block_size > 2 )
			return EXT2_ERR_UNSUPPORTED;
		g_block_size = 1024U << g_superblock.log_block_size;

		uint32_t groups =
		    (g_superblock.blocks_count + g_superblock.blocks_per_group - 1)
		    / g_superblock.blocks_per_group;

		size_t gd_table_bytes = groups * sizeof(ext2_group_desc_t);
		size_t gd_table_blocks =
		    (gd_table_bytes + g_block_size - 1) / g_block_size;

		g_group_desc =
		    (ext2_group_desc_t *) memory::malloc(gd_table_blocks * g_block_size);
		if( !g_group_desc ) {
			return EXT2_ERR_IO;
		}

		/*
	 * Group descriptor table starts right after superblock (block 2 for 1K blk)
	 * For 1 KiB block size the layout is: block0=boot, block1=superblock,
	 * block2=group descriptor table. For 2 KiB/4 KiB, superblock is at
	 * byte 1024 inside block0, so the descriptor table immediately follows
	 * in block1.
	 */
		uint32_t gd_start_blk = (g_block_size == 1024) ? 2 : 1;
		for( uint32_t i = 0; i < gd_table_blocks; i++ ) {
			if( kread_block(gd_start_blk + i,
			                (uint8_t *) g_group_desc + i * g_block_size)
			    != 0 ) {
				return EXT2_ERR_IO;
			}
		}
		return EXT2_OK;
	}

	/**
 * @brief Opens a file or directory by absolute path on the EXT2 filesystem.
 *
 * This function traverses the EXT2 directory hierarchy from the root, following the
 * given absolute path, and retrieves the inode for the target file or directory.
 * The inode structure and an initial file position of 0 are written to @p out_file.
 *
 * The path should be an absolute path (e.g., "/System/Auth/passwd" or "/Home/user").
 * If the path is "/" (the root directory), it opens the root inode.
 *
 * Only direct blocks of directory inodes are used for traversal; indirect directory
 * blocks are not handled.
 *
 * Directories and files can be opened as long as they exist; the function does not
 * check for file types except when traversing intermediate components, which must
 * be directories.
 *
 * @param[in]  path      Absolute path to the file or directory to open (must not be NULL).
 * @param[out] out_file  Pointer to an ext2_file_t structure to receive the opened file
 *                       (must not be NULL).
 *
 * @retval EXT2_OK                 Success. out_file is filled in.
 * @retval EXT2_ERR_INVALID        Invalid argument, or intermediate component is not a directory.
 * @retval EXT2_ERR_PATH_NOT_FOUND Path to target does not exist.
 * @retval EXT2_ERR_IO             An I/O error occurred during traversal.
 */
	int open(const char *path, ext2_file_t *out_file) {
		(void) path;
		(void) out_file;
		if( !path || !out_file )
			return EXT2_ERR_INVALID;

		// If path is just "/" open root directory
		if( kstring::strcmp(path, "/") == 0 ) {
			if( kread_inode(EXT2_ROOT_INODE, &out_file->inode) != EXT2_OK )
				return EXT2_ERR_IO;
			out_file->pos = 0;
			return EXT2_OK;
		}

		// Make a modifiable copy of path without leading '/'
		char   tmp[256];
		size_t plen = (size_t) kstd::strlen(path);
		if( plen >= sizeof(tmp) )
			return EXT2_ERR_INVALID;
		kstring::strcpy(tmp, path[0] == '/' ? path + 1 : path);

		// Current inode when traversing
		ext2_inode_t cur_inode;
		if( kread_inode(EXT2_ROOT_INODE, &cur_inode) != EXT2_OK )
			return EXT2_ERR_IO;

		char *tok = kstring::strtok(tmp, "/");
		char *next_tok;
		while( tok ) {
			next_tok = kstring::strtok(nullptr, "/");

			// Search for tok in cur_inode directory entries (direct blocks only)
			uint32_t found_ino = 0;

			uint8_t *blk_buf = (uint8_t *) memory::malloc(g_block_size);
			if( !blk_buf )
				return EXT2_ERR_IO;

			for( int i = 0; i < 12 && found_ino == 0; i++ ) {
				uint32_t blk = cur_inode.block[i];
				if( blk == 0 )
					continue;
				if( kread_block(blk, blk_buf) != 0 ) {
					memory::free(blk_buf);
					return EXT2_ERR_IO;
				}
				uint32_t off = 0;
				while( off < g_block_size ) {
					ext2_dir_entry_t *ent =
					    (ext2_dir_entry_t *) (blk_buf + off);
					if( ent->rec_len == 0 )
						break;
					if( ent->inode != 0
					    && ent->name_len
					           == (uint8_t) kstd::strlen(tok)
					    && kstring::memcmp(
					           ent->name, tok, ent->name_len)
					           == 0 ) {
						found_ino = ent->inode;
						break;
					}
					off += ent->rec_len;
				}
			}
			memory::free(blk_buf);
			if( found_ino == 0 )
				return EXT2_ERR_PATH_NOT_FOUND;

			// Load inode
			if( kread_inode(found_ino, &cur_inode) != EXT2_OK )
				return EXT2_ERR_IO;

			if( next_tok ) {
				// Expect directory
				if( !(cur_inode.mode & 0x4000) )
					return EXT2_ERR_INVALID;  // Not a directory
			} else {
				// Last component -- should be file (or directory, we allow both)
				out_file->inode = cur_inode;
				out_file->pos   = 0;
				return EXT2_OK;
			}

			tok = next_tok;
		}
		return EXT2_ERR_INVALID;
	}

	/**
 * @brief List the entries in a directory specified by @p path on the EXT2 filesystem.
 *
 * This function traverses the given @p path starting from the root inode, opening each
 * directory in sequence.  When it reaches the final component, it attempts to enumerate
 * all entries inside the corresponding directory using the provided callback @p cb.
 *
 * Only direct blocks (the first 12 data blocks of the directory inode) are processed.
 * Indirect, double indirect, and triple indirect directory data blocks are not parsed.
 *
 * @param[in]  path  The absolute path to the directory to list (e.g., "/etc", "/").
 *                   The path must not be NULL, and must refer to a directory.
 * @param[in]  cb    Callback function invoked for each entry. Must not be NULL.
 *
 * @return EXT2_OK on success, or a negative EXT2_ERR_* code on error:
 *         - EXT2_ERR_INVALID        : Invalid arguments or not a directory.
 *         - EXT2_ERR_IO             : I/O error or out-of-memory.
 *         - EXT2_ERR_PATH_NOT_FOUND : Directory component in path not found.
 */
	int list(const char *path, ext2_list_cb_t cb) {
		if( !path || !cb )
			return EXT2_ERR_INVALID;

		// Special case: root directory
		if( kstring::strcmp(path, "/") == 0 || path[0] == '\0' ) {
			ext2_inode_t root;
			if( kread_inode(EXT2_ROOT_INODE, &root) != EXT2_OK )
				return EXT2_ERR_IO;
			if( !(root.mode & 0x4000) )
				return EXT2_ERR_INVALID;
			return klist_dir_entries(&root, cb);
		}

		char   tmp[256];
		size_t plen = (size_t) kstd::strlen(path);
		if( plen >= sizeof(tmp) )
			return EXT2_ERR_INVALID;
		kstring::strcpy(tmp, path[0] == '/' ? path + 1 : path);

		ext2_inode_t cur_inode;
		if( kread_inode(EXT2_ROOT_INODE, &cur_inode) != EXT2_OK )
			return EXT2_ERR_IO;

		char *tok = kstring::strtok(tmp, "/");
		char *next_tok;
		while( tok ) {
			next_tok = kstring::strtok(nullptr, "/");

			uint32_t found_ino = 0;
			uint8_t *blk_buf   = (uint8_t *) memory::malloc(g_block_size);
			if( !blk_buf )
				return EXT2_ERR_IO;

			for( int i = 0; i < 12 && found_ino == 0; i++ ) {
				uint32_t blk = cur_inode.block[i];
				if( blk == 0 )
					continue;
				if( kread_block(blk, blk_buf) != 0 ) {
					memory::free(blk_buf);
					return EXT2_ERR_IO;
				}
				uint32_t off = 0;
				while( off < g_block_size ) {
					ext2_dir_entry_t *ent =
					    (ext2_dir_entry_t *) (blk_buf + off);
					if( ent->rec_len == 0 )
						break;
					if( ent->inode != 0
					    && ent->name_len
					           == (uint8_t) kstd::strlen(tok)
					    && kstring::memcmp(
					           ent->name, tok, ent->name_len)
					           == 0 ) {
						found_ino = ent->inode;
						break;
					}
					off += ent->rec_len;
				}
			}
			memory::free(blk_buf);
			if( found_ino == 0 )
				return EXT2_ERR_PATH_NOT_FOUND;

			if( kread_inode(found_ino, &cur_inode) != EXT2_OK )
				return EXT2_ERR_IO;

			if( next_tok ) {
				if( !(cur_inode.mode & 0x4000) )
					return EXT2_ERR_INVALID;
			} else {
				if( !(cur_inode.mode & 0x4000) )
					return EXT2_ERR_INVALID;
				return klist_dir_entries(&cur_inode, cb);
			}

			tok = next_tok;
		}
		return EXT2_ERR_INVALID;
	}

	/**
 * @brief List the entries in the root directory of the EXT2 filesystem.
 *
 * This function opens the root directory and lists all directory entries found
 * in its direct blocks.  For each directory entry, it invokes the user-supplied
 * callback function @p cb with the entry's name and file type.
 *
 * Only direct blocks (the first 12 data blocks in the root inode) are processed.
 * Indirect, double indirect, and triple indirect blocks are not parsed.
 *
 * @param[in] cb Callback function to invoke for each directory entry.
 *               Must not be NULL.
 *
 * @return EXT2_OK on success, or a negative EXT2_ERR_* code on error:
 *         - EXT2_ERR_INVALID: Invalid arguments or not a directory.
 *         - EXT2_ERR_IO     : I/O error or out-of-memory.
 */
	int list_dir(ext2_list_cb_t cb) {
		if( !cb )
			return EXT2_ERR_INVALID;

		ext2_inode_t root;
		if( kread_inode(EXT2_ROOT_INODE, &root) != EXT2_OK )
			return EXT2_ERR_IO;

		if( !(root.mode & 0x4000) )  // not a directory
			return EXT2_ERR_INVALID;

		uint8_t *block_buf = (uint8_t *) memory::malloc(g_block_size);
		if( !block_buf )
			return EXT2_ERR_IO;

		// Direct blocks
		for( int i = 0; i < 12; i++ )  // Only direct blocks for now
		{
			uint32_t blk_id = root.block[i];
			if( blk_id == 0 )
				continue;
			if( kread_block(blk_id, block_buf) != 0 ) {
				memory::free(block_buf);
				return EXT2_ERR_IO;
			}
			kprocess_dir_block(block_buf, g_block_size, cb);
		}
		memory::free(block_buf);
		return EXT2_OK;
	}

	/**
 * @brief Read bytes from an open file on the EXT2 filesystem.
 *
 * Reads up to @p len bytes from the file represented by @p file into the buffer @p buf.
 * The read starts at the current file position (file->pos) and updates file->pos by the number
 * of bytes read.  Handles sparse file regions by zero-filling missing blocks according to EXT2 semantics.
 *
 * This function supports files whose size may exceed 4 GiB, using the 64-bit size fields in the inode.
 * Only direct, single, double, and triple indirect block addressing is supported (standard EXT2).
 *
 * @param[in,out] file Pointer to an open EXT2 file structure (must be valid, not NULL).
 * @param[out]    buf  Buffer to receive data (must be at least @p len bytes).
 * @param[in]     len  Number of bytes to read.
 *
 * @return Number of bytes successfully read, 0 if at EOF, or a negative EXT2_ERR_* code on error:
 *         - EXT2_ERR_INVALID    : Invalid arguments or file.
 *         - EXT2_ERR_IO         : Underlying I/O error or out-of-memory during block access.
 *         - EXT2_ERR_UNSUPPORTED: File offset outside of supported block addressable region.
 */
	int read(ext2_file_t *file, void *buf, size_t len) {
		if( !file || !buf )
			return EXT2_ERR_INVALID;

		// Clamp length to remaining bytes in file
		uint64_t size = (uint64_t) file->inode.size_lo
		                | ((uint64_t) file->inode.dir_acl_or_size_high << 32);
		if( file->pos >= size )
			return 0;  // EOF
		if( file->pos + len > size )
			len = size - file->pos;

		uint8_t *dst       = (uint8_t *) buf;
		size_t   remaining = len;

		uint8_t *block_buf = (uint8_t *) memory::malloc(g_block_size);
		if( !block_buf )
			return EXT2_ERR_IO;

		while( remaining > 0 ) {
			uint64_t block_idx    = file->pos / g_block_size;
			uint32_t off_in_block = (uint32_t) (file->pos % g_block_size);

			// Calculate maximum supported blocks
			uint32_t blocks_per_indirect = g_block_size / sizeof(uint32_t);
			uint64_t max_blocks =
			    12 + blocks_per_indirect
			    + (uint64_t) blocks_per_indirect * blocks_per_indirect
			    + (uint64_t) blocks_per_indirect * blocks_per_indirect
			          * blocks_per_indirect;
			// Direct + single indirect + double indirect + triple indirect
			if( block_idx >= max_blocks ) {
				memory::free(block_buf);
				return EXT2_ERR_UNSUPPORTED;
			}

			uint32_t blk_id = kget_file_block(&file->inode, block_idx);
			if( blk_id == 0 ) {
				// Sparse region -- treat as zero
				size_t chunk = g_block_size - off_in_block;
				if( chunk > remaining )
					chunk = remaining;
				kstring::memset(dst, 0, chunk);
				dst += chunk;
				file->pos += chunk;
				remaining -= chunk;
				continue;
			}
			if( kread_block(blk_id, block_buf) != 0 ) {
				memory::free(block_buf);
				return EXT2_ERR_IO;
			}
			size_t chunk = g_block_size - off_in_block;
			if( chunk > remaining )
				chunk = remaining;
			kstring::memcpy(dst, block_buf + off_in_block, chunk);
			dst += chunk;
			file->pos += chunk;
			remaining -= chunk;
		}
		memory::free(block_buf);
		return (int) len;
	}
}  // namespace ext2

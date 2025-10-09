// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/kern/memory/memory_pool.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <kstdio.h>
#include <kstring.h>

#include "memory.h"

#include <dbg/logger.h>

/**
 * @brief Create a memory pool for fixed-size block allocations
 *
 * Allocates a memory pool capable of holding `num_blocks` blocks of size `block_size`.
 * Initializes the pool and its management structures.
 *
 * @param block_size Size of each block in bytes; must be greater than 0
 * @param num_blocks Number of blocks in the pool; must be greater than 0
 * @return Pointer to the created memory_pool_t, or KNULL if allocation fails
 */
memory_pool_t *
    pool_create (ksize_t block_size, ksize_t num_blocks) {
	if (block_size == 0 || num_blocks == 0) {
		return KNULL;
	}

	// Allocate pool structure
	memory_pool_t *pool = kmalloc(sizeof(memory_pool_t));
	if (!pool) {
		return KNULL;
	}

	// Calculate total size needed
	ksize_t total_size = num_blocks * block_size;

	// Allocate the actual pool memory
	pool->pool_start = kmalloc(total_size);
	if (!pool->pool_start) {
		kfree(pool);
		return KNULL;
	}

	// Allocate free list
	pool->free_list = kmalloc(num_blocks * sizeof(void *));
	if (!pool->free_list) {
		kfree(pool->pool_start);
		kfree(pool);
		return KNULL;
	}

	// Initialize pool
	pool->block_size   = block_size;
	pool->total_blocks = num_blocks;
	pool->free_blocks  = num_blocks;

	// Initialize free list
	for (ksize_t i = 0; i < num_blocks; i++) {
		pool->free_list[i] = (kuint8_t *) pool->pool_start + i * block_size;
	}

	return pool;
}

/**
 * @brief Allocate a block from the given memory pool
 *
 * Returns a pointer to a free block from the specified memory pool.
 *
 * @param pool Pointer to a memory_pool_t structure
 * @return Pointer to the allocated block, or KNULL if the pool is exhausted or invalid
 */
void *
    pool_alloc (memory_pool_t *pool) {
	if (!pool || pool->free_blocks == 0) {
		return KNULL;
	}

	// Get next free block
	void *block = pool->free_list[pool->free_blocks - 1];
	pool->free_blocks--;

	return block;
}

/**
 * @brief Free a block previously allocated from the memory pool
 *
 * Validates and returns the memory block to the pool's free list if valid and not already freed.
 *
 * @param pool Pointer to the memory_pool_t the block was allocated from
 * @param ptr Pointer to the memory block to free
 */
void
    pool_free (memory_pool_t *pool, void *ptr) {
	if (!pool || !ptr) {
		return;
	}

	// Check if pointer is within pool bounds
	if (ptr < pool->pool_start
	    || (kuintptr_t) ptr >= (kuintptr_t) pool->pool_start
	                               + pool->total_blocks * pool->block_size) {
		return;  // Invalid pointer
	}

	// Check if pointer is aligned to block size
	kuintptr_t offset = (kuintptr_t) ptr - (kuintptr_t) pool->pool_start;
	if (offset % pool->block_size != 0) {
		return;  // Misaligned pointer
	}

	// Check if block is already free
	for (ksize_t i = 0; i < pool->free_blocks; i++) {
		if (pool->free_list[i] == ptr) {
			return;  // Already free
		}
	}

	// Add to free list
	if (pool->free_blocks < pool->total_blocks) {
		pool->free_list[pool->free_blocks] = ptr;
		pool->free_blocks++;
	}
}

/**
 * @brief Destroy a memory pool and free all associated memory
 *
 * Releases the memory allocated for the pool and its management structures.
 *
 * @param pool Pointer to the memory_pool_t to destroy
 */
void
    pool_destroy (memory_pool_t *pool) {
	if (!pool) {
		return;
	}

	if (pool->pool_start) {
		kfree(pool->pool_start);
	}
	if (pool->free_list) {
		kfree(pool->free_list);
	}
	kfree(pool);
}

// Predefined memory pools for common sizes
static memory_pool_t *small_pool  = KNULL;  // 16 bytes
static memory_pool_t *medium_pool = KNULL;  // 64 bytes
static memory_pool_t *large_pool  = KNULL;  // 256 bytes

/**
 * @brief Initialize the global memory pools for small, medium, and large allocations
 *
 * Creates pools for common allocation sizes (16, 64, and 256 bytes).
 * Logs a warning if any pool creation fails.
 */
void
    init_memory_pools (void) {
	// Create pools for common allocation sizes
	small_pool  = pool_create(16, 1024);  // 16KB total
	medium_pool = pool_create(64, 512);   // 32KB total
	large_pool  = pool_create(256, 128);  // 32KB total

	if (!small_pool || !medium_pool || !large_pool) {
		logger.warn("mm", "Failed to create some memory pools", KNULL);
	}
}

/**
 * @brief Allocate a block from the small memory pool (16 bytes)
 *
 * @return Pointer to the allocated memory block, or KNULL on failure
 */
void *
    pool_alloc_small (void) {
	return pool_alloc(small_pool);
}

/**
 * @brief Allocate a block from the medium memory pool (64 bytes)
 *
 * @return Pointer to the allocated memory block, or KNULL on failure
 */
void *
    pool_alloc_medium (void) {
	return pool_alloc(medium_pool);
}

/**
 * @brief Allocate a block from the large memory pool (256 bytes)
 *
 * @return Pointer to the allocated memory block, or KNULL on failure
 */
void *
    pool_alloc_large (void) {
	return pool_alloc(large_pool);
}

/**
 * @brief Free a block previously allocated from the small memory pool
 *
 * @param ptr Pointer to the memory block to free
 */
void
    pool_free_small (void *ptr) {
	pool_free(small_pool, ptr);
}

/**
 * @brief Free a block previously allocated from the medium memory pool
 *
 * @param ptr Pointer to the memory block to free
 */
void
    pool_free_medium (void *ptr) {
	pool_free(medium_pool, ptr);
}

/**
 * @brief Free a block previously allocated from the large memory pool
 *
 * @param ptr Pointer to the memory block to free
 */
void
    pool_free_large (void *ptr) {
	pool_free(large_pool, ptr);
}

/**
 * @brief Smart memory allocation function that chooses the best pool based on size
 *
 * For allocations <= 256 bytes, chooses one of the predefined memory pools.
 * For larger allocations, uses kmalloc().
 *
 * @param size Size in bytes to allocate
 * @return Pointer to the allocated memory block, or KNULL on failure
 */
void *
    smart_alloc (ksize_t size) {
	if (size <= 16) {
		return pool_alloc_small();
	} else if (size <= 64) {
		return pool_alloc_medium();
	} else if (size <= 256) {
		return pool_alloc_large();
	} else {
		return kmalloc(size);
	}
}

/**
 * @brief Smart memory deallocation function that chooses the correct pool based on size
 *
 * For allocations <= 256 bytes, frees to the appropriate memory pool.
 * For larger allocations, uses kfree().
 *
 * @param ptr  Pointer to the memory block to free
 * @param size Size in bytes that was originally allocated
 */
void
    smart_free (void *ptr, ksize_t size) {
	if (size <= 16) {
		pool_free_small(ptr);
	} else if (size <= 64) {
		pool_free_medium(ptr);
	} else if (size <= 256) {
		pool_free_large(ptr);
	} else {
		kfree(ptr);
	}
}

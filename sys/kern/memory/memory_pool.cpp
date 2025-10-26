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
#include <kstdio.h>
#include <kstring.h>

#include "memory.h"

#include <dbg/logger.h>

namespace memory {
	namespace pool {
		/**
 * @brief Create a memory pool for fixed-size block allocations
 *
 * Allocates a memory pool capable of holding `num_blocks` blocks of size `block_size`.
 * Initializes the pool and its management structures.
 *
 * @param block_size Size of each block in bytes; must be greater than 0
 * @param num_blocks Number of blocks in the pool; must be greater than 0
 * @return Pointer to the created memory_pool_t, or nullptr if allocation fails
 */
		memory_pool_t *create(size_t block_size, size_t num_blocks) {
			if( block_size == 0 || num_blocks == 0 )
				return nullptr;

			// Allocate pool structure
			memory_pool_t *pool =
			    (memory_pool_t *) memory::malloc(sizeof(memory_pool_t));
			if( !pool )
				return nullptr;

			// Calculate total size needed
			size_t total_size = num_blocks * block_size;

			// Allocate the actual pool memory
			pool->pool_start = memory::malloc(total_size);
			if( !pool->pool_start ) {
				memory::free(pool);
				return nullptr;
			}

			// Allocate free list
			pool->free_list =
			    (void **) memory::malloc(num_blocks * sizeof(void *));
			if( !pool->free_list ) {
				memory::free(pool->pool_start);
				memory::free(pool);
				return nullptr;
			}

			// Initialize pool
			pool->block_size   = block_size;
			pool->total_blocks = num_blocks;
			pool->free_blocks  = num_blocks;

			// Initialize free list
			for( size_t i = 0; i < num_blocks; i++ )
				pool->free_list[i] =
				    (uint8_t *) pool->pool_start + i * block_size;

			return pool;
		}

		/**
 * @brief Allocate a block from the given memory pool
 *
 * Returns a pointer to a free block from the specified memory pool.
 *
 * @param pool Pointer to a memory_pool_t structure
 * @return Pointer to the allocated block, or nullptr if the pool is exhausted or invalid
 */
		void *alloc(memory_pool_t *pool) {
			if( !pool || pool->free_blocks == 0 )
				return nullptr;

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
		void free(memory_pool_t *pool, void *ptr) {
			if( !pool || !ptr )
				return;

			// Check if pointer is within pool bounds
			if( ptr < pool->pool_start
			    || (uintptr_t) ptr
			           >= (uintptr_t) pool->pool_start
			                  + pool->total_blocks * pool->block_size ) {
				return;  // Invalid pointer
			}

			// Check if pointer is aligned to block size
			uintptr_t offset = (uintptr_t) ptr - (uintptr_t) pool->pool_start;
			if( offset % pool->block_size != 0 ) {
				return;  // Misaligned pointer
			}

			// Check if block is already free
			for( size_t i = 0; i < pool->free_blocks; i++ ) {
				if( pool->free_list[i] == ptr ) {
					return;  // Already free
				}
			}

			// Add to free list
			if( pool->free_blocks < pool->total_blocks ) {
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
		void destroy(memory_pool_t *pool) {
			if( !pool )
				return;

			if( pool->pool_start )
				memory::free(pool->pool_start);
			if( pool->free_list )
				memory::free(pool->free_list);
			memory::free(pool);
		}

		// Predefined memory pools for common sizes
		static memory_pool_t *small_pool  = nullptr;  // 16 bytes
		static memory_pool_t *medium_pool = nullptr;  // 64 bytes
		static memory_pool_t *large_pool  = nullptr;  // 256 bytes

		/**
 * @brief Initialize the global memory pools for small, medium, and large allocations
 *
 * Creates pools for common allocation sizes (16, 64, and 256 bytes).
 * Logs a warning if any pool creation fails.
 */
		void init_memory(void) {
			// Create pools for common allocation sizes
			small_pool  = create(16, 1024);  // 16KB total
			medium_pool = create(64, 512);   // 32KB total
			large_pool  = create(256, 128);  // 32KB total

			if( !small_pool || !medium_pool || !large_pool ) {
				logger::warn(
				    "mm", "Failed to create some memory pools", nullptr);
			}
		}

		/**
 * @brief Allocate a block from the small memory pool (16 bytes)
 *
 * @return Pointer to the allocated memory block, or nullptr on failure
 */
		void *alloc_small(void) {
			return alloc(small_pool);
		}

		/**
 * @brief Allocate a block from the medium memory pool (64 bytes)
 *
 * @return Pointer to the allocated memory block, or nullptr on failure
 */
		void *alloc_medium(void) {
			return alloc(medium_pool);
		}

		/**
 * @brief Allocate a block from the large memory pool (256 bytes)
 *
 * @return Pointer to the allocated memory block, or nullptr on failure
 */
		void *alloc_large(void) {
			return alloc(large_pool);
		}

		/**
 * @brief Free a block previously allocated from the small memory pool
 *
 * @param ptr Pointer to the memory block to free
 */
		void free_small(void *ptr) {
			free(small_pool, ptr);
		}

		/**
 * @brief Free a block previously allocated from the medium memory pool
 *
 * @param ptr Pointer to the memory block to free
 */
		void free_medium(void *ptr) {
			free(medium_pool, ptr);
		}

		/**
 * @brief Free a block previously allocated from the large memory pool
 *
 * @param ptr Pointer to the memory block to free
 */
		void free_large(void *ptr) {
			free(large_pool, ptr);
		}

		/**
 * @brief Smart memory allocation function that chooses the best pool based on size
 *
 * For allocations <= 256 bytes, chooses one of the predefined memory pools.
 * For larger allocations, uses memory::malloc().
 *
 * @param size Size in bytes to allocate
 * @return Pointer to the allocated memory block, or nullptr on failure
 */
		void *smart_alloc(size_t size) {
			if( size <= 16 ) {
				return alloc_small();
			} else if( size <= 64 ) {
				return alloc_medium();
			} else if( size <= 256 ) {
				return alloc_large();
			} else {
				return memory::malloc(size);
			}
		}

		/**
 * @brief Smart memory deallocation function that chooses the correct pool based on size
 *
 * For allocations <= 256 bytes, frees to the appropriate memory pool.
 * For larger allocations, uses memory::free().
 *
 * @param ptr  Pointer to the memory block to free
 * @param size Size in bytes that was originally allocated
 */
		void smart_free(void *ptr, size_t size) {
			if( size <= 16 ) {
				free_small(ptr);
			} else if( size <= 64 ) {
				free_medium(ptr);
			} else if( size <= 256 ) {
				free_large(ptr);
			} else {
				memory::free(ptr);
			}
		}
	}  // namespace pool
}  // namespace memory

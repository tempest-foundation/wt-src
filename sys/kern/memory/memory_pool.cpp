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
#include "memory.hpp"

#include <kstdio.hpp>
#include <kstring.hpp>
#include <dbg/logger.hpp>

namespace memory {
	namespace pool {
		// ============================================================
		// Helper Functions
		// ============================================================

		/**
		 * @brief Validate memory pool structure
		 * @param pool Pool to validate
		 * @return true if valid, false otherwise
		 */
		static inline bool validate_pool(const memory_pool_t *pool) {
			return pool && pool->magic == POOL_MAGIC
			       && pool->pool_start && pool->free_list;
		}

		/**
		 * @brief Check if pointer belongs to pool
		 * @param pool Pool to check
		 * @param ptr Pointer to check
		 * @return true if ptr is within pool bounds
		 */
		static inline bool is_pool_pointer(const memory_pool_t *pool,
		                                    const void          *ptr) {
			uintptr_t pool_start = (uintptr_t) pool->pool_start;
			uintptr_t pool_end = pool_start + pool->total_blocks * pool->block_size;
			uintptr_t ptr_addr = (uintptr_t) ptr;

			return ptr_addr >= pool_start && ptr_addr < pool_end;
		}

		// ============================================================
		// Pool Management
		// ============================================================

		memory_pool_t *create(size_t block_size, size_t num_blocks) {
			if( block_size == 0 || num_blocks == 0 ) {
				return nullptr;
			}

			// Allocate pool structure
			memory_pool_t *pool =
			    (memory_pool_t *) memory::malloc(sizeof(memory_pool_t));
			if( !pool ) {
				return nullptr;
			}

			// Allocate pool memory
			size_t total_size = num_blocks * block_size;
			pool->pool_start  = memory::malloc(total_size);
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

			// Initialize pool metadata
			pool->block_size   = block_size;
			pool->total_blocks = num_blocks;
			pool->free_blocks  = num_blocks;
			pool->magic        = POOL_MAGIC;

			// Initialize free list with all blocks
			for( size_t i = 0; i < num_blocks; i++ ) {
				pool->free_list[i] =
				    (uint8_t *) pool->pool_start + i * block_size;
			}

			return pool;
		}

		void *alloc(memory_pool_t *pool) {
			if( !validate_pool(pool) ) {
				return nullptr;  // Invalid pool
			}

			if( pool->free_blocks == 0 ) {
				return nullptr;  // Pool exhausted
			}

			// Pop from free list (O(1) operation)
			pool->free_blocks--;
			void *block = pool->free_list[pool->free_blocks];

			return block;
		}

		void free(memory_pool_t *pool, void *ptr) {
			if( !validate_pool(pool) || !ptr ) {
				return;
			}

			// Verify pointer is within pool bounds
			if( !is_pool_pointer(pool, ptr) ) {
				return;  // Pointer not from this pool
			}

			// Verify pointer alignment
			uintptr_t offset = (uintptr_t) ptr - (uintptr_t) pool->pool_start;
			if( offset % pool->block_size != 0 ) {
				return;  // Misaligned pointer (corrupted or invalid)
			}

			// Prevent double-free by checking if pool is already full
			// In a more robust implementation, we'd use a bitmap
			if( pool->free_blocks >= pool->total_blocks ) {
				return;  // All blocks already free (double-free detected)
			}

			// Add to free list (O(1) operation)
			pool->free_list[pool->free_blocks] = ptr;
			pool->free_blocks++;
		}

		void destroy(memory_pool_t *pool) {
			if( !pool ) {
				return;
			}

			// Free all allocated memory
			if( pool->pool_start ) {
				memory::free(pool->pool_start);
			}

			if( pool->free_list ) {
				memory::free(pool->free_list);
			}

			// Invalidate magic number before freeing
			pool->magic = 0;
			memory::free(pool);
		}

		// ============================================================
		// Global Memory Pools
		// ============================================================

		// Predefined pools for common allocation sizes
		static memory_pool_t *small_pool  = nullptr;  // 16 bytes
		static memory_pool_t *medium_pool = nullptr;  // 64 bytes
		static memory_pool_t *large_pool  = nullptr;  // 256 bytes

		void init_memory(void) {
			// Create pools optimized for common allocation patterns
			small_pool  = create(16, 1024);   // 16 KB total
			medium_pool = create(64, 512);    // 32 KB total
			large_pool  = create(256, 256);   // 64 KB total

			if( !small_pool || !medium_pool || !large_pool ) {
				logger::warn("mm",
				             "Failed to initialize some memory pools",
				             nullptr);
			} else {
				logger::info("mm", "Memory pools initialized", nullptr);
			}
		}

		// ============================================================
		// Predefined Pool Allocators
		// ============================================================

		void *alloc_small(void) {
			return alloc(small_pool);
		}

		void *alloc_medium(void) {
			return alloc(medium_pool);
		}

		void *alloc_large(void) {
			return alloc(large_pool);
		}

		void free_small(void *ptr) {
			free(small_pool, ptr);
		}

		void free_medium(void *ptr) {
			free(medium_pool, ptr);
		}

		void free_large(void *ptr) {
			free(large_pool, ptr);
		}

	}  // namespace pool
}  // namespace memory

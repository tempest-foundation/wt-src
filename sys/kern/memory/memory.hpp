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

#include <kstddef.hpp>
#include <kstdint.hpp>

// ============================================================================
// Page and Memory Constants
// ============================================================================

#define PAGE_SIZE             4096ULL
#define PAGE_SIZE_2MB         (2ULL * 1024 * 1024)
#define PAGE_SIZE_1GB         (1024ULL * 1024 * 1024)
#define PAGE_MASK             (PAGE_SIZE - 1)
#define PAGE_ALIGN_UP(addr)   (((addr) + PAGE_MASK) & ~PAGE_MASK)
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~PAGE_MASK)
#define PAGE_INDEX(addr)      ((addr) >> 12)

// Virtual memory layout
#define KERNEL_BASE           0xFFFFFFFF80000000ULL
#define KERNEL_HEAP_BASE      0x01000000ULL  // 16 MB (identity-mapped)
#define KERNEL_HEAP_SIZE      (64ULL * 1024 * 1024)  // 64 MB
#define KERNEL_VMALLOC_BASE   (KERNEL_BASE + 0x10000000ULL)
#define KERNEL_VMALLOC_SIZE   (128ULL * 1024 * 1024)  // 128 MB for vmalloc
#define USER_SPACE_BASE       0x0000000000400000ULL
#define USER_SPACE_SIZE       (0x800000000000ULL - USER_SPACE_BASE)

// ============================================================================
// Page Table Entry Flags
// ============================================================================

#define PAGE_PRESENT       0x001ULL
#define PAGE_WRITABLE      0x002ULL
#define PAGE_USER          0x004ULL
#define PAGE_WRITETHROUGH  0x008ULL
#define PAGE_CACHE_DISABLE 0x010ULL
#define PAGE_ACCESSED      0x020ULL
#define PAGE_DIRTY         0x040ULL
#define PAGE_HUGE          0x080ULL
#define PAGE_GLOBAL        0x100ULL
#define PAGE_NX            0x8000000000000000ULL

// Memory allocation flags
#define MEMORY_ZERO        0x001ULL
#define MEMORY_USER        0x002ULL
#define MEMORY_WRITABLE    0x004ULL
#define MEMORY_EXECUTABLE  0x008ULL

// ============================================================================
// Type Definitions
// ============================================================================

/** Physical memory region types from BIOS/UEFI memory map */
typedef enum {
	MEMORY_USABLE           = 1,
	MEMORY_RESERVED         = 2,
	MEMORY_ACPI_RECLAIMABLE = 3,
	MEMORY_ACPI_NVS         = 4,
	MEMORY_BAD              = 5
} memory_type_t;

/** Memory map entry from bootloader */
typedef struct {
	uint64_t      base_addr;
	uint64_t      length;
	memory_type_t type;
	uint32_t      acpi_attributes;
} memory_map_entry_t;

/** Physical page frame descriptor */
typedef struct page_frame {
	struct page_frame *next;           ///< Next frame in free list
	uint64_t           physical_addr;  ///< Physical address of this page
	uint32_t           ref_count;      ///< Reference counter for shared pages
	uint32_t           flags;          ///< Page flags (reserved, DMA, etc.)
} page_frame_t;

/** Virtual memory region descriptor */
typedef struct vm_region {
	uint64_t          start;   ///< Start virtual address
	uint64_t          end;     ///< End virtual address (exclusive)
	uint64_t          flags;   ///< Protection flags
	struct vm_region *next;    ///< Next region in list
} vm_region_t;

/** Heap block header for kernel heap allocator */
typedef struct heap_block {
	size_t             size;      ///< Usable block size (excluding header)
	uint32_t           magic;     ///< Magic number for validation
	bool               is_free;   ///< Free flag
	struct heap_block *next;      ///< Next block
	struct heap_block *prev;      ///< Previous block
} heap_block_t;

#define HEAP_BLOCK_MAGIC 0xDEADBEEF

/** Memory pool for fixed-size allocations */
typedef struct memory_pool {
	void     *pool_start;    ///< Start of pool memory
	size_t    block_size;    ///< Size of each block
	size_t    total_blocks;  ///< Total number of blocks
	size_t    free_blocks;   ///< Number of free blocks
	void    **free_list;     ///< Array of free block pointers
	uint32_t  magic;         ///< Magic number for validation
} memory_pool_t;

#define POOL_MAGIC 0xDEADC0DE

/** Memory subsystem statistics */
typedef struct {
	uint64_t total_physical_pages;  ///< Total physical memory pages
	uint64_t free_physical_pages;   ///< Available physical pages
	uint64_t used_physical_pages;   ///< Allocated physical pages
	uint64_t total_heap_size;       ///< Total heap size in bytes
	uint64_t free_heap_size;        ///< Free heap space in bytes
	uint64_t used_heap_size;        ///< Used heap space in bytes
	uint64_t heap_blocks;           ///< Number of heap blocks
	uint64_t heap_allocations;      ///< Total allocation count
} memory_stats_t;

// ============================================================================
// Page Table Structures (x86-64 4-level paging)
// ============================================================================

typedef uint64_t pml4e_t;  ///< Page Map Level 4 Entry
typedef uint64_t pml3e_t;  ///< Page Directory Pointer Table Entry
typedef uint64_t pml2e_t;  ///< Page Directory Entry
typedef uint64_t pml1e_t;  ///< Page Table Entry

typedef struct {
	pml4e_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) pml4_t;

typedef struct {
	pml3e_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) pml3_t;

typedef struct {
	pml2e_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) pml2_t;

typedef struct {
	pml1e_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) pml1_t;

// ============================================================================
// Memory Management API
// ============================================================================

namespace memory {
	/**
	 * @brief Initialize the entire memory subsystem
	 * @param multiboot_info Pointer to multiboot2 info structure
	 */
	void init(void *multiboot_info);

	// ========================================================================
	// Physical Memory Management
	// ========================================================================

	/**
	 * @brief Allocate a physical page frame
	 * @return Pointer to page frame descriptor, or nullptr on failure
	 */
	page_frame_t *allocate_page_frame(void);

	/**
	 * @brief Free a physical page frame
	 * @param frame Page frame to free
	 */
	void free_page_frame(page_frame_t *frame);

	/**
	 * @brief Get physical address from page frame
	 * @param frame Page frame descriptor
	 * @return Physical address or 0 on error
	 */
	uint64_t get_physical_addr(page_frame_t *frame);

	/**
	 * @brief Get page frame from physical address
	 * @param physical_addr Physical address
	 * @return Page frame descriptor or nullptr
	 */
	page_frame_t *get_page_frame(uint64_t physical_addr);

	// ========================================================================
	// Kernel Heap Allocation
	// ========================================================================

	/**
	 * @brief Allocate memory from kernel heap
	 * @param size Bytes to allocate
	 * @return Pointer to allocated memory or nullptr
	 */
	void *malloc(size_t size);

	/**
	 * @brief Allocate zeroed memory from kernel heap
	 * @param count Number of elements
	 * @param size Size of each element
	 * @return Pointer to allocated memory or nullptr
	 */
	void *calloc(size_t count, size_t size);

	/**
	 * @brief Resize allocated memory block
	 * @param ptr Original pointer (or nullptr for new allocation)
	 * @param size New size
	 * @return Pointer to reallocated memory or nullptr
	 */
	void *realloc(void *ptr, size_t size);

	/**
	 * @brief Free memory allocated from kernel heap
	 * @param ptr Pointer to free
	 */
	void free(void *ptr);

	// ========================================================================
	// Physical Memory Mapping
	// ========================================================================

	/**
	 * @brief Map physical memory to kernel virtual space
	 * @param physical_addr Physical address to map
	 * @param size Size in bytes
	 * @param flags Page table entry flags
	 * @return Virtual address or nullptr on failure
	 */
	void *map_physical(uint64_t physical_addr, size_t size, uint64_t flags);

	/**
	 * @brief Unmap previously mapped physical memory
	 * @param virtual_addr Virtual address to unmap
	 * @param size Size in bytes
	 */
	void unmap_physical(void *virtual_addr, size_t size);

	// ========================================================================
	// Memory Pools (Fixed-Size Allocators)
	// ========================================================================

	namespace pool {
		/**
		 * @brief Create a new memory pool
		 * @param block_size Size of each block
		 * @param num_blocks Number of blocks
		 * @return Pool handle or nullptr on failure
		 */
		memory_pool_t *create(size_t block_size, size_t num_blocks);

		/**
		 * @brief Allocate a block from pool
		 * @param pool Pool handle
		 * @return Pointer to block or nullptr
		 */
		void *alloc(memory_pool_t *pool);

		/**
		 * @brief Free a block back to pool
		 * @param pool Pool handle
		 * @param ptr Block to free
		 */
		void free(memory_pool_t *pool, void *ptr);

		/**
		 * @brief Destroy a memory pool
		 * @param pool Pool to destroy
		 */
		void destroy(memory_pool_t *pool);

		/** Initialize global memory pools */
		void init_memory(void);

		/** Allocate from small pool (16 bytes) */
		void *alloc_small(void);

		/** Allocate from medium pool (64 bytes) */
		void *alloc_medium(void);

		/** Allocate from large pool (256 bytes) */
		void *alloc_large(void);

		/** Free to small pool */
		void free_small(void *ptr);

		/** Free to medium pool */
		void free_medium(void *ptr);

		/** Free to large pool */
		void free_large(void *ptr);
	}  // namespace pool

	// ========================================================================
	// Virtual Memory Management
	// ========================================================================

	namespace vm {
		/** Initialize virtual memory subsystem */
		void init(void);

		/**
		 * @brief Map a virtual page to physical page
		 * @param virtual_addr Virtual address (page-aligned)
		 * @param physical_addr Physical address (page-aligned)
		 * @param flags Page table flags
		 * @return true on success, false on failure
		 */
		bool map_page(uint64_t virtual_addr,
		              uint64_t physical_addr,
		              uint64_t flags);

		/**
		 * @brief Unmap a virtual page
		 * @param virtual_addr Virtual address to unmap
		 * @return true on success, false if not mapped
		 */
		bool unmap_page(uint64_t virtual_addr);

		/**
		 * @brief Translate virtual address to physical
		 * @param virtual_addr Virtual address
		 * @return Physical address or 0 if not mapped
		 */
		uint64_t get_physical_addr(uint64_t virtual_addr);

		/**
		 * @brief Invalidate TLB entry for page
		 * @param virtual_addr Virtual address
		 */
		void invalidate_page(uint64_t virtual_addr);

		/**
		 * @brief Switch to new page table
		 * @param new_pml4 New PML4 table pointer
		 */
		void switch_pagetable(pml4_t *new_pml4);
	}  // namespace vm

	// ========================================================================
	// Kernel Heap Management
	// ========================================================================

	namespace heap {
		/** Initialize kernel heap */
		void init(void);

		/** Coalesce adjacent free blocks */
		void coalesce(void);
	}  // namespace heap

	// ========================================================================
	// Memory Statistics
	// ========================================================================

	namespace stats {
		/**
		 * @brief Get memory statistics
		 * @return Statistics structure
		 */
		memory_stats_t get(void);

		/** Print memory statistics to console */
		void print(void);
	}  // namespace stats

}  // namespace memory

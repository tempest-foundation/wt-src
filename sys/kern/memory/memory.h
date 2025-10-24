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
#include <kstdint.h>

// Memory constants
#define PAGE_SIZE             4096
#define PAGE_SIZE_2MB         (2 * 1024 * 1024)
#define PAGE_SIZE_1GB         (1024 * 1024 * 1024)
#define PAGE_MASK             (PAGE_SIZE - 1)
#define PAGE_ALIGN_UP(addr)   (((addr) + PAGE_MASK) & ~PAGE_MASK)
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~PAGE_MASK)
#define PAGE_INDEX(addr)      ((addr) >> 12)

// Virtual memory layout
#define KERNEL_BASE      0xFFFFFFFF80000000
#define KERNEL_HEAP_BASE 0x01000000  // 16 MB, identity-mapped in early paging setup
#define KERNEL_HEAP_SIZE (64 * 1024 * 1024)  // 64MB
#define USER_SPACE_BASE  0x0000000000400000
#define USER_SPACE_SIZE  (0x800000000000 - USER_SPACE_BASE)

// Page table entry flags
#define PAGE_PRESENT       0x001
#define PAGE_WRITABLE      0x002
#define PAGE_USER          0x004
#define PAGE_WRITETHROUGH  0x008
#define PAGE_CACHE_DISABLE 0x010
#define PAGE_ACCESSED      0x020
#define PAGE_DIRTY         0x040
#define PAGE_HUGE          0x080
#define PAGE_GLOBAL        0x100
#define PAGE_NX            0x8000000000000000

// Memory allocation flags
#define MEMORY_ZERO       0x001
#define MEMORY_USER       0x002
#define MEMORY_WRITABLE   0x004
#define MEMORY_EXECUTABLE 0x008

// Physical memory regions
typedef enum {
	MEMORY_USABLE           = 1,
	MEMORY_RESERVED         = 2,
	MEMORY_ACPI_RECLAIMABLE = 3,
	MEMORY_ACPI_NVS         = 4,
	MEMORY_BAD              = 5
} memory_type_t;

typedef struct {
	uint64_t      base_addr;
	uint64_t      length;
	memory_type_t type;
	uint32_t      acpi_attributes;
} memory_map_entry_t;

// Page frame structure
typedef struct page_frame {
	struct page_frame *next;
	uint64_t           physical_addr;
	uint32_t           ref_count;
	bool               is_free;
} page_frame_t;

// Virtual memory region
typedef struct vm_region {
	uint64_t          start;
	uint64_t          end;
	uint64_t          flags;
	struct vm_region *next;
} vm_region_t;

// Heap block header
typedef struct heap_block {
	size_t             size;
	bool               is_free;
	struct heap_block *next;
	struct heap_block *prev;
} heap_block_t;

// Memory pool
typedef struct memory_pool {
	void  *pool_start;
	size_t block_size;
	size_t total_blocks;
	size_t free_blocks;
	void **free_list;
} memory_pool_t;

// Memory statistics
typedef struct {
	uint64_t total_physical_pages;
	uint64_t free_physical_pages;
	uint64_t used_physical_pages;
	uint64_t total_heap_size;
	uint64_t free_heap_size;
	uint64_t used_heap_size;
} memory_stats_t;

// Page table structures
typedef uint64_t pml4e_t;
typedef uint64_t pml3e_t;
typedef uint64_t pml2e_t;
typedef uint64_t pml1e_t;

typedef struct {
	pml4e_t entries[512];
} pml4_t;

typedef struct {
	pml3e_t entries[512];
} pml3_t;

typedef struct {
	pml2e_t entries[512];
} pml2_t;

typedef struct {
	pml1e_t entries[512];
} pml1_t;

namespace memory {
	namespace pool {
		memory_pool_t *create(size_t block_size, size_t num_blocks);
		void          *alloc(memory_pool_t *pool);
		void           free(memory_pool_t *pool, void *ptr);
		void           destroy(memory_pool_t *pool);
		void           init_memory(void);
		void          *alloc_small(void);
		void          *alloc_medium(void);
		void          *alloc_large(void);
		void           free_small(void *ptr);
		void           free_medium(void *ptr);
		void           free_large(void *ptr);
		void          *smart_alloc(size_t size);
		void           smart_free(void *ptr, size_t size);
	}  // namespace pool

	namespace vm {
		void     init(void);
		bool     map_page(uint64_t virtual_addr,
		                  uint64_t physical_addr,
		                  uint64_t flags);
		bool     unmap_page(uint64_t virtual_addr);
		uint64_t get_physical_addr(uint64_t virtual_addr);
		void     invalidate_page(uint64_t virtual_addr);
		void     switch_pagetable(pml4_t *new_pml4);
	}  // namespace vm

	namespace heap {
		void init(void);
		void defrag(void);
	}  // namespace heap

	namespace stats {
		memory_stats_t get(void);
		void           print(void);
	}  // namespace stats

	void          init(void *multiboot_info);
	page_frame_t *allocate_page_frame(void);
	void          free_page_frame(page_frame_t *frame);
	uint64_t      get_physical_addr(page_frame_t *frame);
	page_frame_t *get_page_frame(uint64_t physical_addr);

	void *malloc(size_t size);
	void *calloc(size_t count, size_t size);
	void *realloc(void *ptr, size_t size);
	void  free(void *ptr);

	void *map_physical(uint64_t physical_addr, size_t size, uint64_t flags);
	void  unmap_physical(void *virtual_addr, size_t size);

}  // namespace memory

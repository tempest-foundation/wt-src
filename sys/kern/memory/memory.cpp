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

#include <kern/panic/panic.hpp>

// ============================================================================
// Multiboot2 Structures
// ============================================================================

struct multiboot_tag {
	uint32_t type;
	uint32_t size;
};

struct multiboot_tag_mmap {
	uint32_t type;
	uint32_t size;
	uint32_t entry_size;
	uint32_t entry_version;
};

#define MULTIBOOT_TAG_TYPE_MMAP 6

// ============================================================================
// Global Memory State
// ============================================================================

// Physical memory management
static page_frame_t *free_page_list = nullptr;
static page_frame_t *page_frames    = nullptr;
static uint64_t      total_pages    = 0;
static uint64_t      free_pages     = 0;
static uint64_t      used_pages     = 0;

// Kernel heap state
static heap_block_t *heap_start        = nullptr;
static heap_block_t *heap_end          = nullptr;
static uint64_t      heap_size         = 0;
static uint64_t      heap_used         = 0;
static uint64_t      heap_block_count  = 0;
static uint64_t      heap_alloc_count  = 0;

// Virtual memory
static pml4_t *current_pml4 = nullptr;

// Memory map from bootloader
static memory_map_entry_t *memory_map         = nullptr;
static uint32_t            memory_map_entries = 0;

// Constants
static constexpr size_t HEAP_MIN_SPLIT_SIZE = 128;  // Minimum size to split a block
static constexpr size_t HEAP_ALIGNMENT      = 16;   // Heap alignment requirement

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Validate heap block integrity
 * @param block Block to validate
 * @return true if valid, false otherwise
 */
static inline bool validate_heap_block(const heap_block_t *block) {
	return block && block->magic == HEAP_BLOCK_MAGIC;
}

/**
 * @brief Align size to heap alignment requirement
 * @param size Size to align
 * @return Aligned size
 */
static inline size_t align_heap_size(size_t size) {
	return (size + HEAP_ALIGNMENT - 1) & ~(HEAP_ALIGNMENT - 1);
}

// ============================================================================
// Memory Subsystem Initialization
// ============================================================================

namespace memory {
	void init(void *multiboot_info) {
		if( !multiboot_info ) {
			panic::init(1, nullptr);
			return;
		}

		// Parse multiboot2 memory map
		uint32_t total_size  = *(uint32_t *) multiboot_info;
		uint8_t *current_tag = (uint8_t *) ((uintptr_t) multiboot_info + 8);
		uint8_t *end_tags    = (uint8_t *) ((uintptr_t) multiboot_info + total_size);

		// Locate memory map tag
		while( current_tag < end_tags ) {
			struct multiboot_tag *tag = (struct multiboot_tag *) current_tag;

			if( tag->type == 0 ) {
				break;  // End of tags
			}

			if( tag->type == MULTIBOOT_TAG_TYPE_MMAP ) {
				struct multiboot_tag_mmap *mmap_tag =
				    (struct multiboot_tag_mmap *) tag;
				memory_map = (memory_map_entry_t *) ((uint8_t *) mmap_tag
				                                     + sizeof(struct multiboot_tag_mmap));
				memory_map_entries =
				    (mmap_tag->size - sizeof(struct multiboot_tag_mmap))
				    / mmap_tag->entry_size;
				break;
			}

			// Align to 8-byte boundary
			current_tag += (tag->size + 7) & ~0x7ULL;
		}

		if( !memory_map || memory_map_entries == 0 ) {
			panic::init(2, nullptr);  // No memory map found
			return;
		}

		// Calculate total usable pages
		total_pages = 0;
		for( uint32_t i = 0; i < memory_map_entries; i++ ) {
			if( memory_map[i].type == MEMORY_USABLE
			    && memory_map[i].length > 0 ) {
				total_pages += memory_map[i].length / PAGE_SIZE;
			}
		}

		if( total_pages == 0 ) {
			panic::init(3, nullptr);  // No usable memory
			return;
		}

		free_pages = total_pages;
		used_pages = 0;

		// Place page frame array at a safe location after kernel data
		// Use 32MB to be safe (well above kernel .data/.bss at ~17MB)
		page_frames = (page_frame_t *) 0x02000000ULL;  // 32MB

		// Initialize page frame descriptors with ACTUAL physical addresses from memory map
		uint64_t frame_index = 0;
		for( uint32_t i = 0; i < memory_map_entries; i++ ) {
			if( memory_map[i].type != MEMORY_USABLE || memory_map[i].length == 0 ) {
				continue;
			}

			uint64_t region_start = memory_map[i].base_addr;
			uint64_t region_end   = memory_map[i].base_addr + memory_map[i].length;
			
			// Align region to page boundaries
			uint64_t page_start = (region_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
			uint64_t page_end   = region_end & ~(PAGE_SIZE - 1);

			// Create page frames for each page in this usable region
			for( uint64_t addr = page_start; addr < page_end; addr += PAGE_SIZE ) {
				if( frame_index >= total_pages ) {
					break;
				}
				
				page_frames[frame_index].next          = nullptr;
				page_frames[frame_index].physical_addr = addr;
				page_frames[frame_index].ref_count     = 0;
				page_frames[frame_index].flags         = 0;
				frame_index++;
			}
		}

		// Build free list, excluding kernel memory and page frame array
		free_page_list = nullptr;
		uint64_t page_frames_size = total_pages * sizeof(page_frame_t);
		uint64_t page_frames_end = 0x02000000ULL + page_frames_size;
		
		for( uint64_t i = 0; i < total_pages; i++ ) {
			// Skip pages used by kernel code/data and page frame array
			// This includes everything from 0 to end of page_frames array
			if( page_frames[i].physical_addr < page_frames_end ) {
				page_frames[i].ref_count = 1;  // Mark as used
				used_pages++;
				free_pages--;
				continue;
			}

			// Add to free list
			page_frames[i].next = free_page_list;
			free_page_list      = &page_frames[i];
		}

		// Initialize subsystems
		vm::init();
		heap::init();
	}

	// ====================================================================
	// Physical Memory Management
	// ====================================================================

	page_frame_t *allocate_page_frame(void) {
		if( !free_page_list ) {
			return nullptr;  // Out of physical memory
		}

		page_frame_t *frame = free_page_list;
		free_page_list      = frame->next;

		frame->next      = nullptr;
		frame->ref_count = 1;

		free_pages--;
		used_pages++;

		return frame;
	}

	void free_page_frame(page_frame_t *frame) {
		if( !frame || frame->ref_count == 0 ) {
			return;  // Invalid or already free
		}

		frame->ref_count--;
		if( frame->ref_count > 0 ) {
			return;  // Still referenced by others
		}

		// Return to free list
		frame->next    = free_page_list;
		free_page_list = frame;

		free_pages++;
		used_pages--;
	}

	uint64_t get_physical_addr(page_frame_t *frame) {
		return frame ? frame->physical_addr : 0;
	}

	page_frame_t *get_page_frame(uint64_t physical_addr) {
		uint64_t page_index = PAGE_INDEX(physical_addr);
		if( page_index >= total_pages ) {
			return nullptr;
		}
		return &page_frames[page_index];
	}

	namespace vm {
		// Virtual memory management
		void init(void) {
			// Get current page table from CR3 (physical address)
			// Use identity mapping since first 4GB is identity-mapped by bootloader
			uint64_t cr3;
			__asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
			uint64_t pml4_phys = cr3 & ~(uint64_t) 0xFFF;
			// First 4GB is identity-mapped, so physical addr = virtual addr
			current_pml4 = (pml4_t *) pml4_phys;
		}

		bool map_page(uint64_t virtual_addr,
		              uint64_t physical_addr,
		              uint64_t flags) {
			if( (virtual_addr & 0xFFF) || (physical_addr & 0xFFF) )
				return false;

			uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
			uint64_t pml3_index = (virtual_addr >> 30) & 0x1FF;
			uint64_t pml2_index = (virtual_addr >> 21) & 0x1FF;
			uint64_t pml1_index = (virtual_addr >> 12) & 0x1FF;

			pml3_t *pml3 = nullptr;
			if( !(current_pml4->entries[pml4_index] & PAGE_PRESENT) ) {
				page_frame_t *frame = allocate_page_frame();
				if( !frame )
					return false;

				uint64_t phys = memory::get_physical_addr(frame);
				pml3          = (pml3_t *) phys;
				kstring::memset(pml3, 0, PAGE_SIZE);

				current_pml4->entries[pml4_index] =
				    phys | PAGE_PRESENT | PAGE_WRITABLE | (flags & PAGE_USER);
			} else {
				uint64_t pml3_phys =
				    current_pml4->entries[pml4_index] & ~(uint64_t) 0xFFF;
				pml3 = (pml3_t *) pml3_phys;
				// Update flags if needed (e.g., add USER flag)
				if( flags & PAGE_USER ) {
					current_pml4->entries[pml4_index] |= PAGE_USER;
				}
			}

			pml2_t *pml2 = nullptr;
			if( !(pml3->entries[pml3_index] & PAGE_PRESENT) ) {
				page_frame_t *frame = memory::allocate_page_frame();
				if( !frame )
					return false;

				uint64_t phys = memory::get_physical_addr(frame);
				pml2          = (pml2_t *) phys;
				kstring::memset(pml2, 0, PAGE_SIZE);

				pml3->entries[pml3_index] =
				    phys | PAGE_PRESENT | PAGE_WRITABLE | (flags & PAGE_USER);
			} else {
				uint64_t pml2_phys =
				    pml3->entries[pml3_index] & ~(uint64_t) 0xFFF;
				pml2 = (pml2_t *) pml2_phys;
				// Update flags if needed (e.g., add USER flag)
				if( flags & PAGE_USER ) {
					pml3->entries[pml3_index] |= PAGE_USER;
				}
			}

			pml1_t *pml1 = nullptr;
			if( !(pml2->entries[pml2_index] & PAGE_PRESENT) ) {
				page_frame_t *frame = memory::allocate_page_frame();
				if( !frame )
					return false;

				uint64_t phys = memory::get_physical_addr(frame);
				pml1          = (pml1_t *) phys;
				kstring::memset(pml1, 0, PAGE_SIZE);

				pml2->entries[pml2_index] =
				    phys | PAGE_PRESENT | PAGE_WRITABLE | (flags & PAGE_USER);
			} else {
				uint64_t pml1_phys =
				    pml2->entries[pml2_index] & ~(uint64_t) 0xFFF;
				pml1 = (pml1_t *) pml1_phys;
				// Update flags if needed (e.g., add USER flag)
				if( flags & PAGE_USER ) {
					pml2->entries[pml2_index] |= PAGE_USER;
				}
			}

			if( pml1->entries[pml1_index] & PAGE_PRESENT ) {
				// return false;
			}

			// Update page table entry (allow remapping)
			pml1->entries[pml1_index] = physical_addr | flags | PAGE_PRESENT;

			__asm__ volatile("" ::: "memory");

			invalidate_page(virtual_addr);

			return true;
		}

		bool unmap_page(uint64_t virtual_addr) {
			uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
			uint64_t pml3_index = (virtual_addr >> 30) & 0x1FF;
			uint64_t pml2_index = (virtual_addr >> 21) & 0x1FF;
			uint64_t pml1_index = (virtual_addr >> 12) & 0x1FF;

			if( !(current_pml4->entries[pml4_index] & PAGE_PRESENT) ) {
				return false;
			}

			uint64_t pml3_phys =
			    current_pml4->entries[pml4_index] & ~(uint64_t) 0xFFF;
			pml3_t *pml3 = (pml3_t *) pml3_phys;

			if( !(pml3->entries[pml3_index] & PAGE_PRESENT) ) {
				return false;
			}

			uint64_t pml2_phys =
			    pml3->entries[pml3_index] & ~(uint64_t) 0xFFF;
			pml2_t *pml2 = (pml2_t *) pml2_phys;

			if( !(pml2->entries[pml2_index] & PAGE_PRESENT) ) {
				return false;
			}

			uint64_t pml1_phys =
			    pml2->entries[pml2_index] & ~(uint64_t) 0xFFF;
			pml1_t *pml1 = (pml1_t *) pml1_phys;

			if( !(pml1->entries[pml1_index] & PAGE_PRESENT) ) {
				return false;
			}

			// Get physical address and free the page frame
			uint64_t physical_addr =
			    pml1->entries[pml1_index] & ~(uint64_t) 0xFFF;
			page_frame_t *frame = get_page_frame(physical_addr);
			if( frame ) {
				free_page_frame(frame);
			}

			// Clear the page table entry
			pml1->entries[pml1_index] = 0;

			// Invalidate TLB entry
			invalidate_page(virtual_addr);

			return true;
		}

		uint64_t get_physical_addr(uint64_t virtual_addr) {
			uint64_t pml4_index = (virtual_addr >> 39) & 0x1FF;
			uint64_t pml3_index = (virtual_addr >> 30) & 0x1FF;
			uint64_t pml2_index = (virtual_addr >> 21) & 0x1FF;
			uint64_t pml1_index = (virtual_addr >> 12) & 0x1FF;

			if( !(current_pml4->entries[pml4_index] & PAGE_PRESENT) ) {
				return 0;
			}

			uint64_t pml3_phys =
			    current_pml4->entries[pml4_index] & ~(uint64_t) 0xFFF;
			pml3_t *pml3 = (pml3_t *) pml3_phys;

			if( !(pml3->entries[pml3_index] & PAGE_PRESENT) ) {
				return 0;
			}

			uint64_t pml2_phys =
			    pml3->entries[pml3_index] & ~(uint64_t) 0xFFF;
			pml2_t *pml2 = (pml2_t *) pml2_phys;

			if( !(pml2->entries[pml2_index] & PAGE_PRESENT) ) {
				return 0;
			}

			uint64_t pml1_phys =
			    pml2->entries[pml2_index] & ~(uint64_t) 0xFFF;
			pml1_t *pml1 = (pml1_t *) pml1_phys;

			if( !(pml1->entries[pml1_index] & PAGE_PRESENT) ) {
				return 0;
			}

			return (pml1->entries[pml1_index] & ~(uint64_t) 0xFFF)
			       | (virtual_addr & 0xFFF);
		}

		void invalidate_page(uint64_t virtual_addr) {
			__asm__ volatile("invlpg (%0)" : : "r"(virtual_addr) : "memory");
		}

		void switch_pagetable(pml4_t *new_pml4) {
			current_pml4 = new_pml4;
			__asm__ volatile("mov %0, %%cr3"
			                 :
			                 : "r"((uint64_t) new_pml4)
			                 : "memory");
		}
	}  // namespace vm

	// ====================================================================
	// Kernel Heap Management
	// ====================================================================

	namespace heap {
		void init(void) {
			heap_start        = (heap_block_t *) KERNEL_HEAP_BASE;
			heap_size         = KERNEL_HEAP_SIZE;
			heap_used         = 0;
			heap_block_count  = 1;
			heap_alloc_count  = 0;

			// Initialize first free block
			heap_start->size    = heap_size - sizeof(heap_block_t);
			heap_start->magic   = HEAP_BLOCK_MAGIC;
			heap_start->is_free = true;
			heap_start->next    = nullptr;
			heap_start->prev    = nullptr;

			heap_end = heap_start;
		}

		void coalesce(void) {
			// Coalesce adjacent free blocks
			heap_block_t *current = heap_start;

			while( current && current->next ) {
				if( current->is_free && current->next->is_free ) {
					// Merge current with next
					current->size += sizeof(heap_block_t) + current->next->size;
					current->next = current->next->next;

					if( current->next ) {
						current->next->prev = current;
					} else {
						heap_end = current;
					}

					heap_block_count--;
				} else {
					current = current->next;
				}
			}
		}
	}  // namespace heap

	// ====================================================================
	// Memory Statistics
	// ====================================================================

	namespace stats {
		memory_stats_t get(void) {
			memory_stats_t stats;
			stats.total_physical_pages = total_pages;
			stats.free_physical_pages  = free_pages;
			stats.used_physical_pages  = used_pages;
			stats.total_heap_size      = heap_size;
			stats.free_heap_size       = heap_size - heap_used;
			stats.used_heap_size       = heap_used;
			stats.heap_blocks          = heap_block_count;
			stats.heap_allocations     = heap_alloc_count;
			return stats;
		}

		void print(void) {
			memory_stats_t stats = get();
			kstd::printf("=== Memory Statistics ===\n");
			kstd::printf("Physical Memory:\n");
			kstd::printf("  Total:  %llu pages (%llu MiB)\n",
			             stats.total_physical_pages,
			             (stats.total_physical_pages * PAGE_SIZE) / (1024 * 1024));
			kstd::printf("  Free:   %llu pages (%llu MiB)\n",
			             stats.free_physical_pages,
			             (stats.free_physical_pages * PAGE_SIZE) / (1024 * 1024));
			kstd::printf("  Used:   %llu pages (%llu MiB)\n",
			             stats.used_physical_pages,
			             (stats.used_physical_pages * PAGE_SIZE) / (1024 * 1024));
			kstd::printf("\nKernel Heap:\n");
			kstd::printf("  Total:       %llu bytes (%llu MiB)\n",
			             stats.total_heap_size,
			             stats.total_heap_size / (1024 * 1024));
			kstd::printf("  Used:        %llu bytes (%llu MiB)\n",
			             stats.used_heap_size,
			             stats.used_heap_size / (1024 * 1024));
			kstd::printf("  Free:        %llu bytes (%llu MiB)\n",
			             stats.free_heap_size,
			             stats.free_heap_size / (1024 * 1024));
			kstd::printf("  Blocks:      %llu\n", stats.heap_blocks);
			kstd::printf("  Allocations: %llu\n", stats.heap_allocations);
		}
	}  // namespace stats

	// ====================================================================
	// Heap Allocation Functions
	// ====================================================================

	void *malloc(size_t size) {
		if( size == 0 ) {
			return nullptr;
		}

		// Align size to heap alignment
		size_t aligned_size = align_heap_size(size);

		// First-fit allocation strategy
		heap_block_t *current = heap_start;
		while( current ) {
			if( !validate_heap_block(current) ) {
				// Heap corruption detected
				return nullptr;
			}

			if( current->is_free && current->size >= aligned_size ) {
				// Split block if worth it
				if( current->size >= aligned_size + sizeof(heap_block_t)
				                         + HEAP_MIN_SPLIT_SIZE ) {
					// Create new free block from remainder
					heap_block_t *new_block =
					    (heap_block_t *) ((uint8_t *) current
					                      + sizeof(heap_block_t)
					                      + aligned_size);

					new_block->size    = current->size - aligned_size
					                     - sizeof(heap_block_t);
					new_block->magic   = HEAP_BLOCK_MAGIC;
					new_block->is_free = true;
					new_block->next    = current->next;
					new_block->prev    = current;

					if( current->next ) {
						current->next->prev = new_block;
					} else {
						heap_end = new_block;
					}

					current->next = new_block;
					current->size = aligned_size;

					heap_block_count++;
				}

				// Allocate current block
				current->is_free = false;
				heap_used += current->size;
				heap_alloc_count++;

				return (uint8_t *) current + sizeof(heap_block_t);
			}

			current = current->next;
		}

		return nullptr;  // Out of heap memory
	}

	void *calloc(size_t count, size_t size) {
		// Check for overflow without relying on SIZE_MAX
		const size_t max_size = (size_t) -1;
		if( count && size > max_size / count ) {
			return nullptr;
		}

		size_t total_size = count * size;
		void  *ptr        = malloc(total_size);

		if( ptr ) {
			kstring::memset(ptr, 0, total_size);
		}

		return ptr;
	}

	void *realloc(void *ptr, size_t size) {
		// Standard realloc semantics
		if( !ptr ) {
			return malloc(size);
		}

		if( size == 0 ) {
			free(ptr);
			return nullptr;
		}

		// Validate block
		heap_block_t *block =
		    (heap_block_t *) ((uint8_t *) ptr - sizeof(heap_block_t));

		if( !validate_heap_block(block) ) {
			return nullptr;  // Corrupted block
		}

		size_t aligned_size = align_heap_size(size);

		// If new size fits in current block, just return it
		if( block->size >= aligned_size ) {
			return ptr;
		}

		// Try to expand in-place if next block is free
		if( block->next && block->next->is_free ) {
			size_t combined_size = block->size + sizeof(heap_block_t)
			                       + block->next->size;

			if( combined_size >= aligned_size ) {
				// Merge with next block
				block->size = block->next->size + sizeof(heap_block_t)
				              + block->size;
				block->next = block->next->next;

				if( block->next ) {
					block->next->prev = block;
				} else {
					heap_end = block;
				}

				heap_block_count--;

				// Split if remainder is large enough
				if( block->size >= aligned_size + sizeof(heap_block_t)
				                       + HEAP_MIN_SPLIT_SIZE ) {
					heap_block_t *new_block =
					    (heap_block_t *) ((uint8_t *) block
					                      + sizeof(heap_block_t)
					                      + aligned_size);

					new_block->size    = block->size - aligned_size
					                     - sizeof(heap_block_t);
					new_block->magic   = HEAP_BLOCK_MAGIC;
					new_block->is_free = true;
					new_block->next    = block->next;
					new_block->prev    = block;

					if( block->next ) {
						block->next->prev = new_block;
					} else {
						heap_end = new_block;
					}

					block->next = new_block;
					block->size = aligned_size;

					heap_block_count++;
				}

				heap_used += (block->size - block->size);
				return ptr;
			}
		}

		// Allocate new block and copy
		void *new_ptr = malloc(size);
		if( new_ptr ) {
			size_t copy_size = (block->size < size) ? block->size : size;
			kstring::memcpy(new_ptr, ptr, copy_size);
			free(ptr);
		}

		return new_ptr;
	}

	void free(void *ptr) {
		if( !ptr ) {
			return;
		}

		// Get block header
		heap_block_t *block =
		    (heap_block_t *) ((uint8_t *) ptr - sizeof(heap_block_t));

		// Validate block
		if( !validate_heap_block(block) ) {
			return;  // Invalid block, possible corruption
		}

		if( block->is_free ) {
			return;  // Double-free detected
		}

		// Mark as free
		block->is_free = true;
		heap_used -= block->size;

		// Coalesce with next block if free
		if( block->next && block->next->is_free ) {
			block->size += sizeof(heap_block_t) + block->next->size;
			block->next = block->next->next;

			if( block->next ) {
				block->next->prev = block;
			} else {
				heap_end = block;
			}

			heap_block_count--;
		}

		// Coalesce with previous block if free
		if( block->prev && block->prev->is_free ) {
			block->prev->size += sizeof(heap_block_t) + block->size;
			block->prev->next = block->next;

			if( block->next ) {
				block->next->prev = block->prev;
			} else {
				heap_end = block->prev;
			}

			heap_block_count--;
		}
	}

	// ====================================================================
	// Physical Memory Mapping
	// ====================================================================

	void *map_physical(uint64_t physical_addr, size_t size, uint64_t flags) {
		if( size == 0 ) {
			return nullptr;
		}

		// Align physical address and size to page boundaries
		uint64_t phys_aligned = PAGE_ALIGN_DOWN(physical_addr);
		size_t   offset       = physical_addr - phys_aligned;
		size_t   size_aligned = PAGE_ALIGN_UP(size + offset);

		// Use KERNEL_VMALLOC_BASE for temporary mappings
		// In a real implementation, this would use a vmalloc allocator
		uint64_t virtual_addr = KERNEL_VMALLOC_BASE;

		// Map all pages
		size_t num_pages = size_aligned / PAGE_SIZE;
		for( size_t i = 0; i < num_pages; i++ ) {
			uint64_t page_virt = virtual_addr + i * PAGE_SIZE;
			uint64_t page_phys = phys_aligned + i * PAGE_SIZE;

			if( !vm::map_page(page_virt, page_phys, flags | PAGE_PRESENT) ) {
				// Rollback on failure
				for( size_t j = 0; j < i; j++ ) {
					vm::unmap_page(virtual_addr + j * PAGE_SIZE);
				}
				return nullptr;
			}
		}

		// Return virtual address with original offset
		return (void *) (virtual_addr + offset);
	}

	void unmap_physical(void *virtual_addr, size_t size) {
		if( !virtual_addr || size == 0 ) {
			return;
		}

		// Align to page boundaries
		uint64_t virt_aligned = PAGE_ALIGN_DOWN((uint64_t) virtual_addr);
		size_t   offset       = (uint64_t) virtual_addr - virt_aligned;
		size_t   size_aligned = PAGE_ALIGN_UP(size + offset);

		// Unmap all pages
		size_t num_pages = size_aligned / PAGE_SIZE;
		for( size_t i = 0; i < num_pages; i++ ) {
			vm::unmap_page(virt_aligned + i * PAGE_SIZE);
		}
	}

}  // namespace memory

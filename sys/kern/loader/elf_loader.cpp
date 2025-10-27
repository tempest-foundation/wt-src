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
#include "elf_loader.hpp"

#include <kstring.hpp>

#include <dbg/logger.hpp>
#include <kern/memory/memory.hpp>

namespace elf_loader {
// ============================================================================
// Public Interface Implementation
// ============================================================================

bool is_valid_elf(const void *elf_data) {
	if( !elf_data ) {
		return false;
	}

	const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *) elf_data;

	// Verify ELF magic number
	if( ehdr->e_ident[EI_MAG0] != ELFMAG0 || 
	    ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2 || 
	    ehdr->e_ident[EI_MAG3] != ELFMAG3 ) {
		return false;
	}

	// Verify 64-bit ELF
	if( ehdr->e_ident[EI_CLASS] != ELFCLASS64 ) {
		return false;
	}

	return true;
}

int load_elf(const void *elf_data, size_t size, uint64_t *entry_point) {
	(void) size;
	
	// Validate ELF file
	if( !is_valid_elf(elf_data) ) {
		logger::error("elf", "Invalid ELF file", nullptr);
		return -1;
	}

	const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *) elf_data;
	const uint8_t *base = (const uint8_t *) elf_data;

	logger::debug::printf("elf", "info",
	                      "Loading ELF: entry=0x%llx, phnum=%u\n",
	                      ehdr->e_entry, ehdr->e_phnum);

	// Process all program headers
	for( uint16_t i = 0; i < ehdr->e_phnum; i++ ) {
		const Elf64_Phdr *phdr = (const Elf64_Phdr *) 
		    (base + ehdr->e_phoff + i * ehdr->e_phentsize);

		// Only process loadable segments
		if( phdr->p_type != PT_LOAD ) {
			continue;
		}

		logger::debug::printf("elf", "info",
		                      "Loading segment: vaddr=0x%llx, memsz=0x%llx\n",
		                      phdr->p_vaddr, phdr->p_memsz);

		// Calculate page-aligned region
		uint64_t page_start = phdr->p_vaddr & ~0xFFFULL;
		uint64_t page_end = (phdr->p_vaddr + phdr->p_memsz + 0xFFF) & ~0xFFFULL;
		uint64_t num_pages = (page_end - page_start) / PAGE_SIZE;

		// Allocate tracking array for physical page addresses
		uint64_t *phys_pages = 
		    (uint64_t *) memory::malloc(num_pages * sizeof(uint64_t));
		if( !phys_pages ) {
			logger::error("elf", "Failed to allocate page tracking", nullptr);
			return -1;
		}

		// Allocate and map all required pages
		for( uint64_t j = 0; j < num_pages; j++ ) {
			page_frame_t *frame = memory::allocate_page_frame();
			if( !frame ) {
				logger::error("elf", "Failed to allocate page", nullptr);
				memory::free(phys_pages);
				return -1;
			}

			uint64_t virt_addr = page_start + j * PAGE_SIZE;
			uint64_t phys_addr = memory::get_physical_addr(frame);
			phys_pages[j] = phys_addr;

			// Map page with user-accessible permissions (but kernel can still access)
			uint64_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
			memory::vm::map_page(virt_addr, phys_addr, flags);

			// Zero-initialize page via user virtual address (kernel can access user pages)
			kstring::memset((void *)virt_addr, 0, PAGE_SIZE);
		}

		// Copy segment data from ELF file to mapped pages
		if( phdr->p_filesz > 0 ) {
			uint64_t bytes_remaining = phdr->p_filesz;
			uint64_t src_offset = 0;
			uint64_t dst_virt = phdr->p_vaddr;

			while( bytes_remaining > 0 ) {
				// Calculate which page contains this virtual address
				uint64_t page_index = (dst_virt - page_start) / PAGE_SIZE;
				if( page_index >= num_pages ) {
					logger::error("elf", "Virtual address out of range", nullptr);
					memory::free(phys_pages);
					return -1;
				}

				uint64_t page_offset = dst_virt & 0xFFF;
				uint64_t bytes_in_page = PAGE_SIZE - page_offset;
				
				if( bytes_in_page > bytes_remaining ) {
					bytes_in_page = bytes_remaining;
				}

				// Copy data directly to user virtual address (kernel can access it)
				void *dst_ptr = (void *) dst_virt;
				const void *src = base + phdr->p_offset + src_offset;
				kstring::memcpy(dst_ptr, src, bytes_in_page);

				src_offset += bytes_in_page;
				dst_virt += bytes_in_page;
				bytes_remaining -= bytes_in_page;
			}
		}

		// Clean up page tracking array
		memory::free(phys_pages);
	}

	// Return entry point address
	*entry_point = ehdr->e_entry;
	logger::debug::printf("elf", "info", 
	                      "ELF loaded successfully at 0x%llx\n", 
	                      *entry_point);

	return 0;
}

}  // namespace elf_loader
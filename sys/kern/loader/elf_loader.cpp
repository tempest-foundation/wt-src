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
#include "elf_loader.h"

#include <kstring.h>

#include <dbg/logger.h>
#include <kern/memory/memory.h>

namespace elf_loader {
	bool is_valid_elf(const void *elf_data) {
		if( !elf_data )
			return false;

		const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *) elf_data;

		// Check magic number
		if( ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1
		    || ehdr->e_ident[EI_MAG2] != ELFMAG2
		    || ehdr->e_ident[EI_MAG3] != ELFMAG3 ) {
			return false;
		}

		// Check for 64-bit
		if( ehdr->e_ident[EI_CLASS] != ELFCLASS64 ) {
			return false;
		}

		return true;
	}

	// Access physical memory (assumes identity mapping in kernel space)
	static inline void *phys_to_virt(uint64_t phys) {
		// Since KERNEL_HEAP_BASE (0x01000000) is identity-mapped,
		// physical addresses below 4GB should be directly accessible
		if( phys < 0x100000000ULL ) {  // Below 4GB
			return (void *) phys;
		}
		// For higher physical addresses, use kernel offset
		return (void *) (phys + KERNEL_BASE);
	}

	int load_elf(const void *elf_data, size_t size, uint64_t *entry_point) {
		(void) size;
		if( !is_valid_elf(elf_data) ) {
			logger::error("elf", "Invalid ELF file", nullptr);
			return -1;
		}

		const Elf64_Ehdr *ehdr = (const Elf64_Ehdr *) elf_data;
		const uint8_t    *base = (const uint8_t *) elf_data;

		logger::debug::printf("elf",
		                      "info",
		                      "Loading ELF: entry=0x%llx, phnum=%u\n",
		                      ehdr->e_entry,
		                      ehdr->e_phnum);

		// Process program headers
		for( uint16_t i = 0; i < ehdr->e_phnum; i++ ) {
			const Elf64_Phdr *phdr =
			    (const Elf64_Phdr *) (base + ehdr->e_phoff
			                          + i * ehdr->e_phentsize);

			if( phdr->p_type != PT_LOAD ) {
				continue;
			}

			logger::debug::printf(
			    "elf",
			    "info",
			    "Loading segment: vaddr=0x%llx, memsz=0x%llx\n",
			    phdr->p_vaddr,
			    phdr->p_memsz);

			// Calculate number of pages needed
			uint64_t page_start = phdr->p_vaddr & (uint64_t) ~0xFFF;
			uint64_t page_end =
			    (phdr->p_vaddr + phdr->p_memsz + 0xFFF) & (uint64_t) ~0xFFF;
			uint64_t num_pages = (page_end - page_start) / PAGE_SIZE;

			// Track physical addresses for each virtual page
			uint64_t *phys_pages =
			    (uint64_t *) memory::malloc(num_pages * sizeof(uint64_t));
			if( !phys_pages ) {
				logger::error(
				    "elf", "Failed to allocate page tracking", nullptr);
				return -1;
			}

			// Allocate and map pages
			for( uint64_t j = 0; j < num_pages; j++ ) {
				page_frame_t *frame = memory::allocate_page_frame();
				if( !frame ) {
					logger::error(
					    "elf", "Failed to allocate page", nullptr);
					memory::free(phys_pages);
					return -1;
				}

				uint64_t virt = page_start + j * PAGE_SIZE;
				uint64_t phys = memory::get_physical_addr(frame);
				phys_pages[j] = phys;

				// Map with user permissions
				uint64_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
				memory::vm::map_page(virt, phys, flags);

				// Zero the physical page
				void *phys_ptr = phys_to_virt(phys);
				kstring::memset(phys_ptr, 0, PAGE_SIZE);
			}

			// Copy segment data
			if( phdr->p_filesz > 0 ) {
				uint64_t bytes_left = phdr->p_filesz;
				uint64_t src_offset = 0;
				uint64_t dst_virt   = phdr->p_vaddr;

				while( bytes_left > 0 ) {
					// Find which page this virtual address is in
					uint64_t page_index =
					    (dst_virt - page_start) / PAGE_SIZE;
					if( page_index >= num_pages ) {
						logger::error(
						    "elf",
						    "Virtual address out of range",
						    nullptr);
						memory::free(phys_pages);
						return -1;
					}

					uint64_t phys          = phys_pages[page_index];
					uint64_t page_offset   = dst_virt & 0xFFF;
					uint64_t bytes_in_page = PAGE_SIZE - page_offset;
					if( bytes_in_page > bytes_left ) {
						bytes_in_page = bytes_left;
					}

					// Copy through physical address
					void *dst_phys =
					    (void *) ((uintptr_t) phys_to_virt(phys)
					              + page_offset);
					kstring::memcpy(
					    dst_phys,
					    base + phdr->p_offset + src_offset,
					    bytes_in_page);

					src_offset += bytes_in_page;
					dst_virt += bytes_in_page;
					bytes_left -= bytes_in_page;
				}
			}

			// Free the physical page tracking array
			memory::free(phys_pages);
		}

		*entry_point = ehdr->e_entry;
		logger::debug::printf(
		    "elf", "info", "ELF loaded successfully at 0x%llx\n", *entry_point);

		return 0;
	}
}  // namespace elf_loader
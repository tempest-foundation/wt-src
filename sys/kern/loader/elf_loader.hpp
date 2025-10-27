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
// ELF Constants and Magic Numbers
// ============================================================================

// ELF identification indices
#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4

// ELF magic numbers
#define ELFMAG0    0x7f
#define ELFMAG1    'E'
#define ELFMAG2    'L'
#define ELFMAG3    'F'

// ELF classes
#define ELFCLASS64 2

// Program header segment types
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4

// ============================================================================
// ELF Data Structures
// ============================================================================

// ELF64 file header
typedef struct {
	uint8_t  e_ident[16];  // Magic number and other info
	uint16_t e_type;       // Object file type
	uint16_t e_machine;    // Architecture
	uint32_t e_version;    // Object file version
	uint64_t e_entry;      // Entry point virtual address
	uint64_t e_phoff;      // Program header table file offset
	uint64_t e_shoff;      // Section header table file offset
	uint32_t e_flags;      // Processor-specific flags
	uint16_t e_ehsize;     // ELF header size in bytes
	uint16_t e_phentsize;  // Program header table entry size
	uint16_t e_phnum;      // Program header table entry count
	uint16_t e_shentsize;  // Section header table entry size
	uint16_t e_shnum;      // Section header table entry count
	uint16_t e_shstrndx;   // Section header string table index
} __attribute__((packed)) Elf64_Ehdr;

// ELF64 program header
typedef struct {
	uint32_t p_type;    // Segment type
	uint32_t p_flags;   // Segment flags
	uint64_t p_offset;  // Segment file offset
	uint64_t p_vaddr;   // Segment virtual address
	uint64_t p_paddr;   // Segment physical address
	uint64_t p_filesz;  // Segment size in file
	uint64_t p_memsz;   // Segment size in memory
	uint64_t p_align;   // Segment alignment
} __attribute__((packed)) Elf64_Phdr;

// ============================================================================
// ELF Loader Interface
// ============================================================================

namespace elf_loader {

/*
 * Validate ELF file format
 * Returns: true if valid ELF64 file, false otherwise
 */
bool is_valid_elf(const void *elf_data);

/*
 * Load ELF binary into memory
 * Args:
 *   elf_data    - Pointer to ELF file data
 *   size        - Size of ELF file
 *   entry_point - Output pointer for entry point address
 * Returns: 0 on success, -1 on error
 */
int load_elf(const void *elf_data, size_t size, uint64_t *entry_point);

}  // namespace elf_loader

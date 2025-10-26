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
// SPDX-License-Identifier: GPL-3.0-only
/*
 * sys/kern/loader/elf_loader.h
 * ELF binary loader for user-space programs
 */
#pragma once

#include <kstddef.h>
#include <kstdint.h>

// ELF64 Header
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

// ELF64 Program Header
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

// Program header types
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4

// ELF magic numbers
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define EI_CLASS   4
#define ELFCLASS64 2

namespace elf_loader {
	int  load_elf(const void *elf_data, size_t size, uint64_t *entry_point);
	bool is_valid_elf(const void *elf_data);
}  // namespace elf_loader

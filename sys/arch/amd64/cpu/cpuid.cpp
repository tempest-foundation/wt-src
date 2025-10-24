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
#include "cpuid.h"

#include <kmemcpy.h>
#include <kstddef.h>
#include <kstdint.h>

char     cpu_brand_string[CPU_BRAND_STRING_LEN + 1] = "Unknown CPU";
char     cpu_vendor_string[13]                      = "Unknown";  // 12 + null termination
uint32_t cpu_core_id                                = 0;

static inline void
    cpuid(uint32_t eax, uint32_t ecx, uint32_t *regs) {
	__asm__ volatile("cpuid"
	                 : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
	                 : "a"(eax), "c"(ecx));
}

void
    cpu_init_vendor(void) {
	uint32_t regs[4];
	cpuid(0, 0, regs);
	// Vendor: EBX, EDX, ECX
	string::memcpy(cpu_vendor_string + 0, &regs[1], 4);
	string::memcpy(cpu_vendor_string + 4, &regs[3], 4);
	string::memcpy(cpu_vendor_string + 8, &regs[2], 4);
	cpu_vendor_string[12] = '\0';
}

void
    cpu_init_brand(void) {
	uint32_t regs[4];
	cpuid(0x80000000, 0, regs);
	if( regs[0] < 0x80000004 )
		return;

	char *brand = cpu_brand_string;
	for( uint32_t i = 0; i < CPU_BRAND_STRING_LEN / 16; ++i ) {
		cpuid(0x80000002 + i, 0, regs);
		for( int j = 0; j < 4; ++j ) {
			string::memcpy(
			    brand + i * 16 + j * 4, &regs[j], sizeof(uint32_t));
		}
	}
	cpu_brand_string[CPU_BRAND_STRING_LEN] = '\0';
}

uint32_t
    cpu_get_core_id(void) {
	// TODO: Implement SMP (Symmetric Multiprocessing)
	// For now, use a stub
	return 0;
}

void
    cpu_init(void) {
	cpu_init_vendor();
	cpu_init_brand();
	cpu_core_id = cpu_get_core_id();
}

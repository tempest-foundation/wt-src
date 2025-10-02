// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/arch/amd64/cpu/cpuid.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "cpuid.h"

#include <kmemcpy.h>
#include <kstddef.h>
#include <kstdint.h>

#include <kern/init/main.h>

#define CPUID_EXT_BASE 0x80000002

char cpu_brand_string[CPU_BRAND_STRING_LEN + 1] = "Unknown CPU";

static inline void
    cpuid (kuint32_t eax, kuint32_t ecx, kuint32_t *restrict regs) {
	__asm__ volatile("cpuid"
	                 : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
	                 : "a"(eax), "c"(ecx));
}

void
    kcpu_init_brand (void) {
	kuint32_t regs[4];
	cpuid(0x80000000, 0, regs);
	if (regs[0] < 0x80000004)
		return;
	char *brand = cpu_brand_string;
	for (kuint32_t i = 0; i < CPU_BRAND_STRING_LEN / 16; ++i) {
		cpuid(CPUID_EXT_BASE + i, 0, regs);
		for (int j = 0; j < 4; ++j) {
			kmemcpy(brand + i * 16 + j * 4, &regs[j], sizeof(kuint32_t));
		}
	}

	cpu_brand_string[CPU_BRAND_STRING_LEN] = '\0';
}

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
#include "cpuid.hpp"

#include <kmemcpy.hpp>
#include <kstddef.hpp>
#include <kstdint.hpp>

char     cpu_brand_string[CPU_BRAND_STRING_LEN + 1] = "Unknown CPU";
char     cpu_vendor_string[13]                      = "Unknown";  // 12 + null termination
uint32_t cpu_core_id                                = 0;

namespace amd64::cpuid {
	static inline void cpuid(uint32_t eax, uint32_t ecx, uint32_t *regs) {
		__asm__ volatile(
		    "cpuid"
		    : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
		    : "a"(eax), "c"(ecx));
	}

	uint32_t get_core_id(void) {
		// TODO: Implement SMP (Symmetric Multiprocessing)
		// For now, use a stub
		return 0;
	}

	namespace instr {
		/**
 * @brief Checks if the CPU has the SSE2 instruction
 * @return True if the CPU does have, false if not
 */
		bool has_sse2(void) {
			uint32_t regs[4];
			cpuid(1, 0, regs);
			return regs[3] & (1 << 26);  // EDX bit 26 = SSE2
		}

		/**
 * @brief Checks if the CPU has the FPU instruction
 * @return True if the CPU does have, false if not
 */
		bool has_fpu(void) {
			uint32_t regs[4];
			cpuid(1, 0, regs);
			return regs[3] & (1 << 0);  // EDX bit 0 = FPU is present
		}
	}  // namespace instr

	namespace vendor {
		void init(void) {
			uint32_t regs[4];
			cpuid(0, 0, regs);
			// Vendor: EBX, EDX, ECX
			kstring::memcpy(cpu_vendor_string + 0, &regs[1], 4);
			kstring::memcpy(cpu_vendor_string + 4, &regs[3], 4);
			kstring::memcpy(cpu_vendor_string + 8, &regs[2], 4);
			cpu_vendor_string[12] = '\0';
		}
	}  // namespace vendor

	namespace brand {
		void init(void) {
			uint32_t regs[4];
			cpuid(0x80000000, 0, regs);
			if( regs[0] < 0x80000004 )
				return;

			char *brand = cpu_brand_string;
			for( uint32_t i = 0; i < CPU_BRAND_STRING_LEN / 16; ++i ) {
				cpuid(0x80000002 + i, 0, regs);
				for( int j = 0; j < 4; ++j ) {
					kstring::memcpy(brand + i * 16 + j * 4,
					                &regs[j],
					                sizeof(uint32_t));
				}
			}
			cpu_brand_string[CPU_BRAND_STRING_LEN] = '\0';
		}
	}  // namespace brand

	void init(void) {
		vendor::init();
		brand::init();
		cpu_core_id = get_core_id();
	}
}  // namespace amd64::cpuid
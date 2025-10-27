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

#include <kstdint.hpp>

#define CPU_BRAND_STRING_LEN 64
extern char     cpu_brand_string[CPU_BRAND_STRING_LEN + 1];
extern char     cpu_vendor_string[13];
extern uint32_t cpu_core_id;

namespace amd64::cpuid {
	namespace instr {
		bool has_sse2(void);
		bool has_fpu(void);
	}  // namespace instr

	namespace vendor {
		void init(void);
	}

	namespace brand {
		void init(void);
	}

	void init(void);
}  // namespace amd64::cpuid

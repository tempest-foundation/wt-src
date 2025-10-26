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
#include "framebuf.h"

#include <kstdint.h>

#include <dbg/logger.h>

struct framebuffer_info fb_info;

namespace framebuf {
	/**
 * @brief Maps a physical framebuffer address to a virtual address in the kernel space
 * @param phys_addr Physical address of the framebuffer provided by the bootloader
 * @note The virtual address is offset by 0xFFFF800000000000 and stored in `fb_info.addr`
 */
	void map_address(uint64_t phys_addr) {
		uint64_t virt_addr = 0xFFFF800000000000ULL + phys_addr;

		logger::debug::printf("fb",
		                      "info",
		                      "Mapping framebuffer 0x%llx -> 0x%llx\n",
		                      phys_addr,
		                      virt_addr);

		fb_info.addr = virt_addr;
	}
}  // namespace framebuf

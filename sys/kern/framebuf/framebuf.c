#include "framebuf.h"

#include <kstdint.h>

#include <dbg/logger.h>

struct framebuffer_info fb_info;

/**
 * @brief Maps a physical framebuffer address to a virtual address in the kernel space
 * @param phys_addr Physical address of the framebuffer provided by the bootloader
 * @note The virtual address is offset by 0xFFFF800000000000 and stored in `fb_info.addr`
 */
void
    map_framebuffer_address (kuint64_t phys_addr) {
	kuint64_t virt_addr = 0xFFFF800000000000ULL + phys_addr;

	logger.printf(
	    "fb", "info", "Mapping framebuffer 0x%llx -> 0x%llx\n", phys_addr, virt_addr);

	fb_info.addr = virt_addr;
}

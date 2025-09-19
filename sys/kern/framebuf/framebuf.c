#include "framebuf.h"

#include <debug/debug.h>
#include <lib/kstdio/kstdint.h>

struct framebuffer_info fb_info;

void
    map_framebuffer_address (kuint64_t phys_addr) {
	kuint64_t virt_addr = 0xFFFF800000000000ULL + phys_addr;

	debug.printf(
	    "fb", "Mapping framebuffer 0x%llx -> 0x%llx\n", phys_addr, virt_addr);

	fb_info.addr = virt_addr;
}

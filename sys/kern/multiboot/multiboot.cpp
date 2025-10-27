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
#include "multiboot.hpp"

#include <kstdio.hpp>

#include "kern/framebuf/framebuf.hpp"
#ifdef ARCH_AMD64
#	include <arch/amd64/cpu/halt.hpp>
#endif
#include <dbg/logger.hpp>

// Multiboot2 header structure.
struct multiboot_header {
	uint32_t total_size;
	uint32_t reserved;
} __attribute__((aligned(8)));

struct multiboot_tag {
	uint32_t type;
	uint32_t size;
} __attribute__((aligned(8)));

struct multiboot_tag_framebuffer {
	uint32_t type;
	uint32_t size;
	uint64_t addr;
	uint32_t pitch;
	uint32_t width;
	uint32_t height;
	uint8_t  bpp;
	uint8_t  type_fb;
	uint8_t  red_mask_size;
	uint8_t  red_mask_shift;
	uint8_t  green_mask_size;
	uint8_t  green_mask_shift;
	uint8_t  blue_mask_size;
	uint8_t  blue_mask_shift;
	uint8_t  reserved[2];
} __attribute__((aligned(8)));

typedef enum {
	MULTIBOOT_TAG_TYPE_END         = 0,
	MULTIBOOT_TAG_TYPE_FRAMEBUFFER = 8
} multiboot_tag_type_t;

namespace multiboot {
	/**
	 * @brief Parse the multiboot information given by the bootloader
	 * @param mb_info Pointer to the multiboot information structure
	 * given by the bootloader
	 * @warning The whole system depends on the multiboot information,
	 * If none, you are fucked
	 */
	void parse(void *mb_info) {
		if( mb_info == nullptr ) {
			logger::debug::puts(
			    "mb", "error", "mb_info is EMPTY! Halting...");
			amd64::halt();
		}

		uint32_t total_size = *(uint32_t *) mb_info;
		uint8_t *current    = (uint8_t *) ((uintptr_t) mb_info + 8);
		uint8_t *end        = (uint8_t *) ((uintptr_t) mb_info + total_size);

		logger::debug::puts("mb", "info", "Parsing multiboot info...");

		while( current < end ) {
			struct multiboot_tag *tag = (struct multiboot_tag *) current;

			if( tag->size == 0 ) {
				logger::error("mb", "error", "Invalid tag size (0)");
				return;
			}

			switch( (multiboot_tag_type_t) tag->type ) {
				case MULTIBOOT_TAG_TYPE_END:
					logger::debug::puts(
					    "mb",
					    "info",
					    "End tag found. Parsing complete.");
					return;

				case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
					logger::debug::puts(
					    "mb", "info", "Framebuffer tag found");

					struct multiboot_tag_framebuffer *fb_tag =
					    (struct multiboot_tag_framebuffer *) tag;

					fb_info.pitch           = fb_tag->pitch;
					fb_info.width           = fb_tag->width;
					fb_info.height          = fb_tag->height;
					fb_info.bpp             = fb_tag->bpp;
					fb_info.type            = fb_tag->type_fb;
					fb_info.red_mask_size   = fb_tag->red_mask_size;
					fb_info.red_mask_shift  = fb_tag->red_mask_shift;
					fb_info.green_mask_size = fb_tag->green_mask_size;
					fb_info.green_mask_shift =
					    fb_tag->green_mask_shift;
					fb_info.blue_mask_size  = fb_tag->blue_mask_size;
					fb_info.blue_mask_shift = fb_tag->blue_mask_shift;

					framebuf::map_address(fb_tag->addr);
					break;
				}

				default:
					// Unhandled tag
					// ! DO NOT FUCKING PRINT A ERROR MESSAGE
					// ! THIS IS NOT A ERROR, JUST A UNHANDLED TAG
					break;
			}

			current += (tag->size + 7) & (uint32_t) ~7;  // align to 8 bytes
		}

		logger::debug::puts(
		    "mb", "info", "Reached end of multiboot info (no END tag)");
	}
}  // namespace multiboot

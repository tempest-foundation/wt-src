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

// Framebuffer information structure
struct framebuffer_info {
	uint64_t addr;
	uint32_t pitch;
	uint32_t width;
	uint32_t height;
	uint8_t  bpp;
	uint8_t  type;
	uint8_t  red_mask_size;
	uint8_t  red_mask_shift;
	uint8_t  green_mask_size;
	uint8_t  green_mask_shift;
	uint8_t  blue_mask_size;
	uint8_t  blue_mask_shift;
};

namespace video {
	bool     is_ready(void);
	void     clear(uint32_t color);
	uint32_t rgb_to_bgr(uint32_t rgb);
	uint32_t hex_to_color(const char *hex);
	void     init(struct framebuffer_info *fb);
	void     put_pixel(uint32_t x, uint32_t y, uint32_t rgb_color);
	void     putchar(char c);
	void     draw_circle(int cx, int cy, int radius, uint32_t rgb_color);
	void     draw_square(int cx, int cy, int radius, uint32_t rgb_color);
	void     puts(const char *s);
}  // namespace video

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
#include <kunistd.h>

#include <drv/video/video.h>
#include <kern/framebuf/framebuf.h>

void cmd_test_graphics(const char *args) {
	(void) args;

	// Safety check for division by zero
	if( !video::is_ready() )
		return;

	video::clear(0xFFFFFF);

	uint8_t  circle_diff = unistd::rand::unsign() & 0xFF;
	uint32_t circle_x    = fb_info.width / 2;
	uint32_t circle_y    = fb_info.height / 2;
	uint32_t color       = unistd::rand::unsign() & 0xFFFFFF;
	video::draw_circle((int) circle_x, (int) circle_y, 100, color);

	// First square: left
	color              = unistd::rand::unsign() & 0xFFFFFF;
	uint32_t square1_x = circle_x - circle_diff;
	uint32_t square1_y = circle_y;
	video::draw_square((int) square1_x, (int) square1_y, 100, color);

	// Second square: right
	color              = unistd::rand::unsign() & 0xFFFFFF;
	uint32_t square2_x = circle_x + circle_diff;
	uint32_t square2_y = circle_y;
	video::draw_square((int) square2_x, (int) square2_y, 100, color);

	unistd::sleep::sleep(5000);

	video::clear(0x000000);
}

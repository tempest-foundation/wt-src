// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/drv/video/video.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "video.h"

#include <dbg/logger.h>
#include <drv/video/font/font.h>
#include <kern/framebuf/framebuf.h>
#include <kern/init/main.h>
#include <lib/kstdio/kstdbool.h>
#include <lib/kstdio/kstddef.h>

static volatile kuint32_t *framebuffer = KNULL;

static kuint32_t cursor_x = 0;
static kuint32_t cursor_y = 0;

kbool
    video_is_ready (void) {
	return fb_info.addr != 0 && fb_info.width > 0 && fb_info.height > 0;
}

// Convert 24-bit RGB (0xRRGGBB) to 16-bit RGB565
static inline kuint16_t
    video_rgb888_to_rgb565 (kuint32_t rgb) {
	kuint8_t r = (rgb >> 16) & 0xFF;
	kuint8_t g = (rgb >> 8) & 0xFF;
	kuint8_t b = rgb & 0xFF;
	return (kuint16_t) ((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3));
}

void
    video_clear (kuint32_t color) {
	if (!video_is_ready())
		return;

	for (kuint32_t y = 0; y < fb_info.height; y++) {
		volatile kuint8_t *row_base =
		    (volatile kuint8_t *) framebuffer + y * fb_info.pitch;

		if (fb_info.bpp == 16) {
			volatile kuint16_t *row     = (volatile kuint16_t *) row_base;
			kuint16_t           color16 = video_rgb888_to_rgb565(color);
			for (kuint32_t x = 0; x < fb_info.width; x++) {
				row[x] = color16;
			}
		} else if (fb_info.bpp == 24) {
			volatile kuint8_t *row = row_base;
			kuint8_t           r   = (color >> 16) & 0xFF;
			kuint8_t           g   = (color >> 8) & 0xFF;
			kuint8_t           b   = (color >> 0) & 0xFF;
			for (kuint32_t x = 0; x < fb_info.width; x++) {
				row[x * 3 + 0] = r;
				row[x * 3 + 1] = g;
				row[x * 3 + 2] = b;
			}
		} else if (fb_info.bpp == 32) {
			volatile kuint32_t *row = (volatile kuint32_t *) row_base;
			for (kuint32_t x = 0; x < fb_info.width; x++) {
				row[x] = color;
			}
		}
	}

	cursor_x = 0;
	cursor_y = 0;
}

static kbool
    video_is_hex_char (char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

kuint32_t
    video_rgb_to_bgr (kuint32_t rgb) {
	kuint8_t r = (rgb >> 16) & 0xFF;
	kuint8_t g = (rgb >> 8) & 0xFF;
	kuint8_t b = rgb & 0xFF;
	return ((kuint32_t) b << 16) | ((kuint32_t) g << 8) | (kuint32_t) r;
}

kuint32_t
    video_hex_to_color (const char *hex) {
	if (hex[0] == '#')
		hex++;  // Just fucks up the '#'
	if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X'))
		hex += 2;

	kuint32_t value = 0;
	for (int i = 0; i < 6 && video_is_hex_char(hex[i]); i++) {
		char c = hex[i];
		value <<= 4;
		if (c >= '0' && c <= '9')
			value |= (kuint32_t) (c - '0');
		else if (c >= 'a' && c <= 'f')
			value |= (kuint32_t) (c - 'a' + 10);
		else if (c >= 'A' && c <= 'F')
			value |= (kuint32_t) (c - 'A' + 10);
	}

	return value;
}

void
    video_init (struct framebuffer_info *fb) {
	if (!video_is_ready()) {
		return;
	}

	framebuffer = (volatile kuint32_t *) fb->addr;

	cursor_x = 0;
	cursor_y = 0;
}

void
    video_put_pixel (kuint32_t x, kuint32_t y, kuint32_t rgb_color) {
	if (!video_is_ready()) {
		return;
	}

	if (fb_info.bpp == 16) {
		kuint16_t *row =
		    (kuint16_t *) ((kuint8_t *) framebuffer + y * fb_info.pitch);
		row[x] = video_rgb888_to_rgb565(rgb_color);
	} else {
		kuint32_t *row =
		    (kuint32_t *) ((kuint8_t *) framebuffer + y * fb_info.pitch);
		row[x] = video_rgb_to_bgr(rgb_color);
	}
}

static void
    video_draw_glyph_at (char c, kuint32_t x, kuint32_t y, kuint32_t rgb_color) {
	if (!video_is_ready() || (unsigned int) c >= 256) {
		return;
	}

	const unsigned char *glyph = font[(unsigned int) c];

	for (kuint32_t row = 0; row < FONT_HEIGHT; row++) {
		unsigned char row_data = glyph[row];
		for (kuint32_t col = 0; col < FONT_WIDTH; col++) {
			if (row_data & (0x80 >> col)) {
				video_put_pixel(x + col, y + row, rgb_color);
			}
		}
	}
}

void
    video_putchar (char c) {
	if (!video_is_ready())
		return;

	if (c == '\b') {
		if (cursor_x >= FONT_WIDTH) {
			cursor_x -= FONT_WIDTH;
		} else if (cursor_y >= FONT_HEIGHT) {
			cursor_y -= FONT_HEIGHT;
			cursor_x = fb_info.width - FONT_WIDTH;
		} else {
			return;
		}
		for (kuint32_t row = 0; row < FONT_HEIGHT; row++) {
			for (kuint32_t col = 0; col < FONT_WIDTH; col++) {
				video_put_pixel(cursor_x + col, cursor_y + row, 0x000000);
			}
		}
		return;
	}

	if (c == '\n') {
		cursor_x = 0;
		cursor_y += FONT_HEIGHT;
	} else if (c == '\r') {
		cursor_x = 0;
	} else {
		video_draw_glyph_at(c, cursor_x, cursor_y, 0xFFFFFF);
		cursor_x += FONT_WIDTH;
	}

	if (cursor_x + FONT_WIDTH > fb_info.width) {
		cursor_x = 0;
		cursor_y += FONT_HEIGHT;
	}

	if (cursor_y + FONT_HEIGHT > fb_info.height) {
		video_clear(0x000000);
	}
}

void
    video_draw_circle (int cx, int cy, int radius, kuint32_t rgb_color) {
	for (int y = -radius; y <= radius; y++) {
		for (int x = -radius; x <= radius; x++) {
			if (x * x + y * y <= radius * radius) {
				int px = cx + x;
				int py = cy + y;
				if (px >= 0 && py >= 0) {
					video_put_pixel(
					    (kuint32_t) px, (kuint32_t) py, rgb_color);
				}
			}
		}
	}
}

void
    video_draw_square (int cx, int cy, int size, kuint32_t rgb_color) {
	int half = size / 2;

	for (int y = cy - half; y < cy + half; y++) {
		for (int x = cx - half; x < cx + half; x++) {
			if (x >= 0 && y >= 0 && x < (int) fb_info.width
			    && y < (int) fb_info.height) {
				video_put_pixel((kuint32_t) x, (kuint32_t) y, rgb_color);
			}
		}
	}
}

void
    video_puts (const char *s) {
	while (*s) {
		video_putchar(*s++);
	}
}

struct video video = {.init             = video_init,
                      .is_ready         = video_is_ready,
                      .rgb888_to_rgb565 = video_rgb888_to_rgb565,
                      .rgb_to_bgr       = video_rgb_to_bgr,
                      .hex_to_color     = video_hex_to_color,
                      .put_pixel        = video_put_pixel,
                      .clear            = video_clear,
                      .put_char         = video_putchar,
                      .puts             = video_puts,
                      .draw_circle      = video_draw_circle,
                      .draw_square      = video_draw_square};

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
#include "video.h"

#include <kstddef.h>

#include <dbg/logger.h>
#include <drv/video/font/font.h>
#include <kern/framebuf/framebuf.h>

static volatile uint32_t *framebuffer = nullptr;

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

namespace video {
	bool is_ready(void) {
		return fb_info.addr != 0 && fb_info.width > 0 && fb_info.height > 0;
	}

	static inline uint16_t rgb888_to_rgb565(uint32_t rgb) {
		uint8_t r = (rgb >> 16) & 0xFF;
		uint8_t g = (rgb >> 8) & 0xFF;
		uint8_t b = rgb & 0xFF;
		return (uint16_t) ((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3));
	}

	void clear(uint32_t color) {
		if( !is_ready() )
			return;

		uint32_t total_pixels = fb_info.width * fb_info.height;

		switch( fb_info.bpp ) {
			case 16: {
				volatile uint16_t *fb16 =
				    (volatile uint16_t *) framebuffer;
				uint16_t color16 = rgb888_to_rgb565(color);
				for( uint32_t i = 0; i < total_pixels; i++ ) {
					fb16[i] = color16;
				}
				break;
			}
			case 24: {
				volatile uint8_t *fb8 = (volatile uint8_t *) framebuffer;
				uint8_t           r   = (color >> 16) & 0xFF;
				uint8_t           g   = (color >> 8) & 0xFF;
				uint8_t           b   = color & 0xFF;
				for( uint32_t i = 0; i < total_pixels; i++ ) {
					uint32_t offset = i * 3;
					fb8[offset + 0] = r;
					fb8[offset + 1] = g;
					fb8[offset + 2] = b;
				}
				break;
			}
			case 32: {
				volatile uint32_t *fb32 =
				    (volatile uint32_t *) framebuffer;
				for( uint32_t i = 0; i < total_pixels; i++ ) {
					fb32[i] = color;
				}
				break;
			}
			default:
				// Unsupported BPP
				return;
		}

		cursor_x = 0;
		cursor_y = 0;
	}

	static inline bool is_hex_char(char c) {
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
		       || (c >= 'A' && c <= 'F');
	}

	static inline uint8_t hex_char_to_value(char c) {
		if( c >= '0' && c <= '9' )
			return (uint8_t) (c - '0');
		else if( c >= 'a' && c <= 'f' )
			return (uint8_t) (c - 'a' + 10);
		else if( c >= 'A' && c <= 'F' )
			return (uint8_t) (c - 'A' + 10);
		return 0;
	}

	uint32_t rgb_to_bgr(uint32_t rgb) {
		uint8_t r = (rgb >> 16) & 0xFF;
		uint8_t g = (rgb >> 8) & 0xFF;
		uint8_t b = rgb & 0xFF;
		return ((uint32_t) b << 16) | ((uint32_t) g << 8) | (uint32_t) r;
	}

	uint32_t hex_to_color(const char *hex) {
		if( !hex )
			return 0;

		// Skip '#' prefix
		if( hex[0] == '#' )
			hex++;
		// Skip '0x' prefix
		if( hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X') )
			hex += 2;

		uint32_t value = 0;
		for( int i = 0; i < 6 && is_hex_char(hex[i]); i++ ) {
			value = (value << 4) | hex_char_to_value(hex[i]);
		}

		return value;
	}

	void init(struct framebuffer_info *fb) {
		if( !fb || !fb->addr )
			return;

		framebuffer = (volatile uint32_t *) fb->addr;

		cursor_x = 0;
		cursor_y = 0;
	}

	void put_pixel(uint32_t x, uint32_t y, uint32_t rgb_color) {
		if( !is_ready() || x >= fb_info.width || y >= fb_info.height )
			return;

		volatile uint8_t *pixel_addr =
		    (volatile uint8_t *) framebuffer + y * fb_info.pitch;

		switch( fb_info.bpp ) {
			case 16: {
				volatile uint16_t *row = (volatile uint16_t *) pixel_addr;
				row[x]                 = rgb888_to_rgb565(rgb_color);
				break;
			}
			case 24: {
				uint8_t r             = (rgb_color >> 16) & 0xFF;
				uint8_t g             = (rgb_color >> 8) & 0xFF;
				uint8_t b             = rgb_color & 0xFF;
				pixel_addr[x * 3 + 0] = r;
				pixel_addr[x * 3 + 1] = g;
				pixel_addr[x * 3 + 2] = b;
				break;
			}
			case 32: {
				volatile uint32_t *row = (volatile uint32_t *) pixel_addr;
				row[x]                 = rgb_to_bgr(rgb_color);
				break;
			}
		}
	}

	static void draw_glyph_at(char c, uint32_t x, uint32_t y, uint32_t rgb_color) {
		if( !is_ready() )
			return;

		// Bounds check for character and screen position
		unsigned int char_idx = (unsigned int) (unsigned char) c;
		if( char_idx >= 256 || x + FONT_WIDTH > fb_info.width
		    || y + FONT_HEIGHT > fb_info.height )
			return;

		const unsigned char *glyph = font[char_idx];

		for( uint32_t row = 0; row < FONT_HEIGHT; row++ ) {
			unsigned char row_data = glyph[row];
			for( uint32_t col = 0; col < FONT_WIDTH; col++ ) {
				if( row_data & (0x80 >> col) ) {
					put_pixel(x + col, y + row, rgb_color);
				}
			}
		}
	}

	static void scroll(void) {
		if( !is_ready() )
			return;

		// Move all lines up by FONT_HEIGHT pixels
		uint32_t bytes_per_line = fb_info.pitch;
		uint32_t lines_to_move  = fb_info.height - FONT_HEIGHT;

		volatile uint8_t *fb_base = (volatile uint8_t *) framebuffer;

		// Copy lines upward
		for( uint32_t y = 0; y < lines_to_move; y++ ) {
			volatile uint8_t *dest = fb_base + y * bytes_per_line;
			volatile uint8_t *src =
			    fb_base + (y + FONT_HEIGHT) * bytes_per_line;

			for( uint32_t x = 0; x < bytes_per_line; x++ ) {
				dest[x] = src[x];
			}
		}

		// Clear the last line
		for( uint32_t y = lines_to_move; y < fb_info.height; y++ ) {
			volatile uint8_t *row = fb_base + y * bytes_per_line;
			for( uint32_t x = 0; x < bytes_per_line; x++ ) {
				row[x] = 0;
			}
		}

		cursor_y = lines_to_move;
	}

	void putchar(char c) {
		if( !is_ready() )
			return;

		switch( c ) {
			case '\b':
				if( cursor_x >= FONT_WIDTH ) {
					cursor_x -= FONT_WIDTH;
				} else if( cursor_y >= FONT_HEIGHT ) {
					cursor_y -= FONT_HEIGHT;
					cursor_x =
					    (fb_info.width / FONT_WIDTH - 1) * FONT_WIDTH;
				} else {
					return;
				}
				// Clear the character at cursor position
				for( uint32_t row = 0; row < FONT_HEIGHT; row++ ) {
					for( uint32_t col = 0; col < FONT_WIDTH; col++ ) {
						put_pixel(cursor_x + col,
						          cursor_y + row,
						          0x000000);
					}
				}
				break;

			case '\n':
				cursor_x = 0;
				cursor_y += FONT_HEIGHT;
				break;

			case '\r':
				cursor_x = 0;
				break;

			case '\t':
				// Tab support (8 spaces)
				cursor_x = ((cursor_x / FONT_WIDTH + 8) & (uint32_t) ~7)
				           * FONT_WIDTH;
				if( cursor_x >= fb_info.width ) {
					cursor_x = 0;
					cursor_y += FONT_HEIGHT;
				}
				break;

			default:
				if( c >= 32 && c < 127 ) {  // Printable ASCII only
					draw_glyph_at(c, cursor_x, cursor_y, 0xFFFFFF);
					cursor_x += FONT_WIDTH;
				}
				break;
		}

		// Handle line wrapping
		if( cursor_x + FONT_WIDTH > fb_info.width ) {
			cursor_x = 0;
			cursor_y += FONT_HEIGHT;
		}

		// Handle scrolling instead of clearing screen
		if( cursor_y + FONT_HEIGHT > fb_info.height ) {
			scroll();
		}
	}

	void draw_circle(int cx, int cy, int radius, uint32_t rgb_color) {
		for( int y = -radius; y <= radius; y++ ) {
			for( int x = -radius; x <= radius; x++ ) {
				if( x * x + y * y <= radius * radius ) {
					int px = cx + x;
					int py = cy + y;
					if( px >= 0 && py >= 0 ) {
						put_pixel((uint32_t) px,
						          (uint32_t) py,
						          rgb_color);
					}
				}
			}
		}
	}

	void draw_square(int cx, int cy, int size, uint32_t rgb_color) {
		if( !is_ready() || size <= 0 )
			return;

		int half    = size / 2;
		int x_start = cx - half;
		int y_start = cy - half;
		int x_end   = cx + half;
		int y_end   = cy + half;

		// Clamp to screen bounds
		if( x_start < 0 )
			x_start = 0;
		if( y_start < 0 )
			y_start = 0;
		if( x_end > (int) fb_info.width )
			x_end = (int) fb_info.width;
		if( y_end > (int) fb_info.height )
			y_end = (int) fb_info.height;

		for( int y = y_start; y < y_end; y++ ) {
			for( int x = x_start; x < x_end; x++ ) {
				put_pixel((uint32_t) x, (uint32_t) y, rgb_color);
			}
		}
	}

	void puts(const char *s) {
		if( !is_ready() || !s )
			return;

		while( *s ) {
			putchar(*s++);
		}
	}
}  // namespace video

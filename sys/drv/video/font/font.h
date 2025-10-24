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

#include <kstdint.h>

// A simple 8x8 font for rendering text to the framebuffer.
// Each character is 8 pixels wide and 8 pixels tall.
extern const uint32_t FONT_WIDTH;
extern const uint32_t FONT_HEIGHT;

extern const unsigned char font[128][8];

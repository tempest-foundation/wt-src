// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/drv/video/font/font.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <lib/kstdio/kstdint.h>

// A simple 8x8 font for rendering text to the framebuffer.
// Each character is 8 pixels wide and 8 pixels tall.
extern const kuint32_t FONT_WIDTH;
extern const kuint32_t FONT_HEIGHT;

extern const unsigned char font[128][8];

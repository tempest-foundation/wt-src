#pragma once

#include "drivers/video/video.h"

#include <lib/kstdio/kstdint.h>

extern struct framebuffer_info fb_info;

void
    map_framebuffer_address (kuint64_t phys_addr);

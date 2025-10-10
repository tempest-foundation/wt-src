#pragma once

#include <kstdint.h>

#include "drv/video/video.h"

extern struct framebuffer_info fb_info;

void
    map_framebuffer_address (kuint64_t phys_addr);

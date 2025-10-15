#include <kunistd.h>

#include <drv/video/video.h>
#include <kern/framebuf/framebuf.h>

void
    cmd_test_graphics (const char *args) {
	(void) args;

	// Safety check for division by zero
	if (!video.is_ready()) {
		return;
	}

	video.clear(0xFFFFFF);

	kuint8_t  circle_diff = rand32.uns() & 0xFF;
	kuint32_t circle_x    = fb_info.width / 2;
	kuint32_t circle_y    = fb_info.height / 2;
	kuint32_t color       = rand32.uns() & 0xFFFFFF;
	video.draw_circle((int) circle_x, (int) circle_y, 100, color);

	// First square: left
	color               = rand32.uns() & 0xFFFFFF;
	kuint32_t square1_x = circle_x - circle_diff;
	kuint32_t square1_y = circle_y;
	video.draw_square((int) square1_x, (int) square1_y, 100, color);

	// Second square: right
	color               = rand32.uns() & 0xFFFFFF;
	kuint32_t square2_x = circle_x + circle_diff;
	kuint32_t square2_y = circle_y;
	video.draw_square((int) square2_x, (int) square2_y, 100, color);

	ksleep(5000);

	video.clear(0x000000);
}

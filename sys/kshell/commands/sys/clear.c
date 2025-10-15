#include "clear.h"

#include <kerrno.h>
#include <kstdio.h>
#include <kstrtol.h>

#include <drv/video/video.h>

void
    cmd_clear (const char *args) {
	kuint32_t color = 0x000000;
	if (args && *args) {
		int base = 0;
		if (args[0] == '#') {
			args++;
			base = 16;
		}
		kerrno = 0;
		char *end;
		long  val = kstrtol(args, &end, base);
		if (kerrno == 0 && end != args && val >= 0 && val <= 0xFFFFFF)
			color = (kuint32_t) val;
	}
	video.clear(color);
}

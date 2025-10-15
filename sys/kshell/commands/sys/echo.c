#include "echo.h"

#include <kstdio.h>

#include <drv/tty/tty.h>

void
    cmd_echo (const char *args) {
	if (!args || *args == '\0') {
		kputs("Echo... echo... echo...");
		kputs("\n");
		return;
	}
	kputs(args);
	kputs("\n");
}

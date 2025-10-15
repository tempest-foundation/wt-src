#include "history.h"

#include <kstdio.h>

#include <drv/tty/tty.h>
#include <kshell/commands/commands.h>

char command_history[MAX_HISTORY][CMD_BUFFER_SIZE];
int  history_count = 0;

void
    cmd_history (const char *args) {
	(void) args;
	if (history_count == 0) {
		kputs("No commands in history\n");
		return;
	}
	char buf[128];
	for (int i = 0; i < history_count && i < MAX_HISTORY; ++i) {
		ksnprintf(buf, sizeof(buf), "%2d - %s\n", i + 1, command_history[i]);
		kputs(buf);
	}
}

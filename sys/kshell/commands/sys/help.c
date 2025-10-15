#include "help.h"

#include <kstdio.h>
#include <kstring.h>

#include <drv/tty/tty.h>
#include <kshell/commands/commands.h>

void
    cmd_help (const char *args) {
	(void) args;
	kprintf("Available commands\n");

	// collect categories
	const char *categories[16];
	int         num_categories = 0;
	for (ksize_t i = 0; i < NUM_COMMANDS; ++i) {
		int found = 0;
		for (int j = 0; j < num_categories; ++j) {
			if (kstrcmp(commands[i].cat, categories[j]) == 0) {
				found = 1;
				break;
			}
		}
		if (!found)
			categories[num_categories++] = commands[i].cat;
	}

	char buf[128];
	for (int c = 0; c < num_categories; ++c) {
		ksnprintf(buf, sizeof(buf), "\n[%s]\n", categories[c]);
		kprintf(buf);
		for (ksize_t i = 0; i < NUM_COMMANDS; ++i) {
			if (kstrcmp(commands[i].cat, categories[c]) == 0) {
				ksnprintf(buf,
				          sizeof(buf),
				          " %-12s - %s\n",
				          commands[i].name,
				          commands[i].desc);
				kprintf(buf);
			}
		}
	}
}

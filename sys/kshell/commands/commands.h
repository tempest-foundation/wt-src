#pragma once

#include <kstddef.h>

typedef void (*command_func_t)(const char *args);

struct Command {
	const char    *name;
	const char    *desc;
	const char    *cat;
	command_func_t handler;
};

#define CMD_BUFFER_SIZE 512
#define MAX_HISTORY     128

extern struct Command commands[];
extern const ksize_t  NUM_COMMANDS;

int
    handle_command (char *cmd);

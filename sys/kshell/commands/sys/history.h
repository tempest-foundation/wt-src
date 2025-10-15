#pragma once

#include <kshell/commands/commands.h>

extern char command_history[MAX_HISTORY][CMD_BUFFER_SIZE];
extern int  history_count;

void
    cmd_history (const char *args);

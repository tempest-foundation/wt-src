// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#pragma once

#include <kstddef.hpp>

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
extern const size_t   NUM_COMMANDS;

namespace kshell {
	int handle_command(char *cmd);
}

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
#include "commands.h"

#include <kprint.h>
#include <kstrcmp.h>

// Commands import
#include "fs/cat.h"
#include "fs/cd.h"
#include "fs/fsize.h"
#include "fs/ls.h"
#include "fs/pwd.h"
#include "hardware/panic.h"
#include "hardware/poweroff.h"
#include "hardware/reboot.h"
#include "hardware/sleep.h"
#include "info/fetch.h"
#include "info/time.h"
#include "sys/clear.h"
#include "sys/echo.h"
#include "sys/exec.h"
#include "sys/help.h"
#include "sys/history.h"
#include "test/test_graphics.h"

struct Command commands[] = {
    // System
    {"help", "Show this help message", "System", cmd_help},
    {"clear", "Clear the screen", "System", cmd_clear},
    {"echo", "Echo a message", "System", cmd_echo},
    {"history", "Show the history of commands", "System", cmd_history},
    {"exec", "Execute a ELF program", "System", cmd_exec},

    // Hardware
    {"poweroff", "Power off the OS", "Hardware", cmd_poweroff},
    {"reboot", "Reboot the OS", "Hardware", cmd_reboot},
    {"kpanic", "Test kernel kpanic (DANGEROUS!)", "Hardware", cmd_panic},
    {"sleep", "Test the HPET timer", "Hardware", cmd_sleep},

    // Info
    {"fetch", "View system information", "Info", cmd_fetch},
    {"time", "Show current date and time", "Info", cmd_time},

    // Test
    {"test_graphics", "Test the graphics driver", "Test", cmd_test_graphics},

    // Filesystem
    {"ls", "List directory", "Filesystem", cmd_ls},
    {"cat", "Read file from filesystem", "Filesystem", cmd_cat},
    {"fsize", "Show file size", "Filesystem", cmd_fsize},
    {"cd", "Change current directory", "Filesystem", cmd_cd},
    {"pwd", "Print current directory", "Filesystem", cmd_pwd},
};

const size_t NUM_COMMANDS = sizeof(commands) / sizeof(commands[0]);

namespace kshell {
	int handle_command(char *cmd) {
		if( !cmd || !*cmd )
			return -1;

		char *space = cmd;
		while( *space && *space != ' ' )
			++space;

		char *args = nullptr;
		if( *space == ' ' ) {
			*space = '\0';
			args   = space + 1;
			while( *args == ' ' )
				++args;
		}

		for( size_t i = 0; i < NUM_COMMANDS; ++i ) {
			if( kstring::strcmp(cmd, commands[i].name) == 0 ) {
				commands[i].handler(args);
				return 0;
			}
		}

		kstd::printf("Unknown command: '%s'\n", cmd);
		return -1;
	}
}  // namespace kshell
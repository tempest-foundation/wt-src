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
#include "kernSh.h"

#include <kmemcpy.h>
#include <kstdio.h>

#include "commands/commands.h"
#include "commands/sys/history.h"

#include <drv/tty/tty.h>

static char cmd_buffer[CMD_BUFFER_SIZE];
static int  cmd_ptr        = 0;
static int  input_overflow = 0;

/**
 * @brief Runs the kernsh interactive shell loop
 *
 * Presents the user with a shell prompt, reads input from the TTY, processes
 * backspaces and line editing, and dispatches complete commands to the command
 * handler.  Commands are also saved in a history buffer.  The shell runs
 * indefinitely, handling user input and providing responses through TTY.
 * Ensures input does not exceed the buffer size and reports an error on overflow.
 */
void
    kshell(void) {
	kstd::puts(
	    "Copyright (c) Tempest Foundation, 2025\n"
	    "This is free software with ABSOLUTELY NO WARRANTY.\n"
	    "You may redistribute and/or modify it under the terms of\n"
	    "the GNU General Public License Version 3.\n"
	    "Full license in the LICENSE file. Do not use if you disagree.");

	for( ;; ) {
		// ! Don't use `kstd::puts()` for that shit
		// ! It will break the line from the command.
		// * This `tty::write()` is like `fputs()`.
		tty::write("\n[shell@tempest]@ ");
		cmd_ptr        = 0;
		input_overflow = 0;

		for( ;; ) {
			char c = (char) tty::read_char();
			if( c == '\n' ) {
				kstd::putchar('\n');
				if( input_overflow ) {
					kstd::puts("Error: command too long\n");
				} else {
					cmd_buffer[cmd_ptr] = '\0';
					if( cmd_ptr > 0 ) {
						// Save to history
						size_t len =
						    (size_t) kstd::strlen(cmd_buffer);
						if( len >= CMD_BUFFER_SIZE )
							len = CMD_BUFFER_SIZE - 1;
						string::memcpy(
						    command_history[history_count
						                    % MAX_HISTORY],
						    cmd_buffer,
						    len);
						command_history[history_count
						                % MAX_HISTORY][len] =
						    '\0';
						history_count++;

						handle_command(cmd_buffer);
					}
				}
				break;
			} else if( c == '\b' || c == 127 ) {
				if( cmd_ptr > 0 )
					cmd_ptr--;
			} else if( (unsigned char) c >= 32 && (unsigned char) c < 127 ) {
				if( cmd_ptr < CMD_BUFFER_SIZE - 1 )
					cmd_buffer[cmd_ptr++] = c;
				else
					input_overflow = 1;
			}
		}
	}
}

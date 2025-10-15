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
    kshell (void) {
	kputs("Copyright (c) 2025, Tempest Foundation");
	kputs("Type 'help' for a list of commands\n");

	for (;;) {
		// ! Don't use `kputs()` for that shit
		// ! It will break the line from the command.
		// * This `tty.write()` is like `fputs()`.
		tty.write("\n[kernel@tempest]@ ");
		cmd_ptr        = 0;
		input_overflow = 0;

		for (;;) {
			char c = (char) tty.read_char();
			if (c == '\n') {
				kputchar('\n');
				if (input_overflow) {
					kputs("Error: command too long\n");
				} else {
					cmd_buffer[cmd_ptr] = '\0';
					if (cmd_ptr > 0) {
						// Save to history
						ksize_t len =
						    (ksize_t) kstrlen(cmd_buffer);
						if (len >= CMD_BUFFER_SIZE)
							len = CMD_BUFFER_SIZE - 1;
						kmemcpy(command_history[history_count
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
			} else if (c == '\b' || c == 127) {
				if (cmd_ptr > 0)
					cmd_ptr--;
			} else if ((unsigned char) c >= 32 && (unsigned char) c < 127) {
				if (cmd_ptr < CMD_BUFFER_SIZE - 1)
					cmd_buffer[cmd_ptr++] = c;
				else
					input_overflow = 1;
			}
		}
	}
}

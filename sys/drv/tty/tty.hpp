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

#define TTY_BUF_SIZE 1024

struct Main_tty {
	char input_buf[TTY_BUF_SIZE];
	int  head;
	int  tail;
	void (*write_char)(char c);
	int echo;
};

extern struct Main_tty main_tty;

namespace tty {
	void receive_char(char c);
	int  read_char(void);
	void write(const char *s);
	void init(void);
}  // namespace tty

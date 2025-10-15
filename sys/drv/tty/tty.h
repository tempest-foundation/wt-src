#pragma once

#define TTY_BUF_SIZE 1024

struct tty {
	char input_buf[TTY_BUF_SIZE];
	int  head;
	int  tail;
	void (*write_char)(char c);
	int echo;
};

extern struct tty main_tty;

struct tty_driver {
	void (*init)(void);
	void (*write)(const char *s);
	int (*read_char)(void);
	void (*receive_char)(char c);
};

extern struct tty_driver tty;

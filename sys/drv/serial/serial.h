#pragma once

#include <kstdbool.h>

extern struct Serial serial;

struct Serial {
	void (*init)(void);
	void (*write)(char a);
	void (*writes)(const char *s);
	void (*write_int)(int i);
	kbool (*is_available)(void);
};

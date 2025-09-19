#pragma once

#include <lib/kstdio/kstddef.h>

extern struct Vfs vfs;

struct Vfs {
	void (*getcwd)(char *out, ksize_t size);
	void (*normalize)(const char *path, char *out, ksize_t size);
	void (*resolve)(const char *path, char *out, ksize_t size);
	int (*chdir)(const char *path);
};

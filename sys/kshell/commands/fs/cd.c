#include <kprint.h>

#include <fs/vfs/vfs.h>

void
    cmd_cd (const char *args) {
	const char *path = (args && *args) ? args : "/";
	int         rc   = vfs.chdir(path);
	if (rc != 0) {
		kprintf("cd: cannot access %s (err %d)\n", path, rc);
	}
}

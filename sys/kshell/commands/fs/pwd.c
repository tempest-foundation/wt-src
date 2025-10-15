#include <kprint.h>

#include <fs/vfs/vfs.h>

void
    cmd_pwd (const char *args) {
	(void) args;
	char buf[256];
	vfs.getcwd(buf, sizeof(buf));
	kputs(buf);
}

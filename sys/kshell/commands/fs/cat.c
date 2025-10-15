#include <kprint.h>

#include <fs/ext2/ext2.h>
#include <fs/vfs/vfs.h>

void
    cmd_cat (const char *args) {
	if (!args || *args == '\0') {
		kputs("Usage: cat <path>");
		return;
	}
	char        abs_path[256];
	const char *path = args;
	if (path[0] != '/') {
		vfs.resolve(path, abs_path, sizeof(abs_path));
		path = abs_path;
	}
	ext2_file_t file;
	int         rc = ext2.open(path, &file);
	if (rc != 0) {
		kprintf("cat: cannot open %s (err %d)\n", path, rc);
		return;
	}
	char buf[512];
	int  read;
	while ((read = ext2.read(&file, buf, sizeof(buf) - 1)) > 0) {
		buf[read] = '\0';
		kputs(buf);
	}
}

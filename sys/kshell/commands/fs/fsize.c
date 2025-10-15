#include <kprint.h>

#include <fs/ext2/ext2.h>
#include <fs/vfs/vfs.h>

void
    cmd_fsize (const char *args) {
	if (!args || *args == '\0') {
		kputs("Usage: fsize <path>");
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
		kprintf("fsize: cannot open %s (err %d)\n", path, rc);
		return;
	}
	kuint64_t size =
	    (((kuint64_t) file.inode.dir_acl_or_size_high) << 32) | file.inode.size_lo;
	kprintf("%s: %llu bytes\n", args, size);
}

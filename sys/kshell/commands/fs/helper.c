#include <kprint.h>

#include <fs/ext2/ext2.h>

void
    ls_print_cb (const char *name, kuint8_t file_type) {
	(void) file_type;
	kputs(name);
}

void
    list_dir_path (const char *path) {
	int rc = ext2.list(path, ls_print_cb);
	if (rc != 0) {
		kprintf("ls: cannot access %s (err %d)\n", path, rc);
	}
}

#include <lib/kstdio/kstrlen/kstrlen.h>
#include <lib/kstring/kmemcpy.h>
#include <lib/kstring/kstrcpy.h>

char *
    kstrcpy (char *dest, const char *src) {
	ksize_t len = kstrlen(src);
	kmemcpy(dest, src, len + 1);  // +1 for null terminator
	return dest;
}

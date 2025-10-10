#include <kmemcpy.h>
#include <kstrcpy.h>

#include <kstrlen/kstrlen.h>

char *
    kstrcpy (char *dest, const char *src) {
	ksize_t len = kstrlen(src);
	kmemcpy(dest, src, len + 1);  // +1 for null terminator
	return dest;
}

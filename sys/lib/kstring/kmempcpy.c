#include <lib/kstdio/kstddef.h>
#include <lib/kstring/kmempcpy.h>

void *
    kmempcpy (void *dest, const void *src, ksize_t n) {
	char       *d = (char *) dest;
	const char *s = (const char *) src;
	for (ksize_t i = 0; i < n; i++)
		d[i] = s[i];
	return d + n;
}

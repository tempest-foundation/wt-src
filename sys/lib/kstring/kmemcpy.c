#include <lib/kstdio/kstddef.h>
#include <lib/kstring/kmemcpy.h>

void *
    kmemcpy (void *restrict dest, const void *restrict src, ksize_t n) {
	if (!dest || !src || n == 0)
		return dest;
	unsigned char       *d = (unsigned char *) dest;
	const unsigned char *s = (const unsigned char *) src;

	if (s < d && s + n > d) {
		for (ksize_t i = n; i > 0; --i)
			d[i - 1] = s[i - 1];
	} else {
		for (ksize_t i = 0; i < n; ++i)
			d[i] = s[i];
	}
	return dest;
}

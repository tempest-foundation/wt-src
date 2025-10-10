#include <kstddef.h>
#include <kstrncpy.h>

char *
    kstrncpy (char *dest, const char *src, ksize_t n) {
	char       *d = dest;
	const char *s = src;

	while (n && *s) {
		*d++ = *s++;
		n--;
	}

	while (n--) {
		*d++ = '\0';
	}

	return dest;
}

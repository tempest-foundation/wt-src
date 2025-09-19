#include <lib/kstring/kstrcat.h>

char *
    kstrcat (char *dest, const char *src) {
	if (!dest || !src)
		return dest;

	// Find the end of dest
	char *end = dest;
	while (*end) {
		end++;
	}

	// Copy src to the end of dest
	while (*src) {
		*end = *src;
		end++;
		src++;
	}

	*end = '\0';
	return dest;
}

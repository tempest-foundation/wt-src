#include <lib/kstdio/kstddef.h>
#include <lib/kstring/kstrchr.h>

char *
    kstrchr (const char *str, int c) {
	while (*str != '\0') {
		if (*str == (char) c) {
			return (char *) str;
		}
		str++;
	}

	if (c == '\0') {
		return (char *) str;
	}

	return KNULL;
}

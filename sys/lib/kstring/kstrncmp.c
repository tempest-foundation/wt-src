#include <lib/kstdio/kstddef.h>
#include <lib/kstring/kstrncmp.h>

int
    kstrncmp (const char *s1, const char *s2, ksize_t n) {
	for (ksize_t i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return (unsigned char) s1[i] - (unsigned char) s2[i];
		}
		if (s1[i] == '\0') {
			return 0;
		}
	}
	return 0;
}

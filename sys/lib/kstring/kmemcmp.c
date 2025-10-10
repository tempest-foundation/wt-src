#include <kmemcmp.h>
#include <kstddef.h>

int
    kmemcmp (const void *s1, const void *s2, ksize_t n) {
	const unsigned char *a = s1;
	const unsigned char *b = s2;
	for (ksize_t i = 0; i < n; ++i) {
		if (a[i] != b[i])
			return (int) a[i] - (int) b[i];
	}
	return 0;
}

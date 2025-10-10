#include <kstddef.h>
#include <kstrlen.h>

ksize_t
    kstrlen (const char *str) {
	if (!str)
		return 0;

	const char *s = str;
	while (*s)
		s++;
	return (ksize_t) (s - str);
}

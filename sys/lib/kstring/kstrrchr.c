#include <kstddef.h>
#include <kstrrchr.h>

const char *
    kstrrchr (const char *str, int c) {
	if (!str)
		return KNULL;

	const char  ch   = (char) c;
	const char *last = KNULL;

	do {
		if (*str == ch)
			last = str;
	} while (*str++);

	return last;
}

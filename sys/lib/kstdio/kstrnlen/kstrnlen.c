#include <lib/kstdio/kstddef.h>
#include <lib/kstdio/kstrnlen/kstrnlen.h>

ksize_t
    kstrnlen (const char *str, ksize_t maxlen) {
	ksize_t len = 0;
	while (len < maxlen && str[len] != '\0') {
		len++;
	}
	return len;
}

#include <kstddef.h>
#include <kstrnlen.h>

ksize_t
    kstrnlen (const char *str, ksize_t maxlen) {
	ksize_t len = 0;
	while (len < maxlen && str[len] != '\0') {
		len++;
	}
	return len;
}

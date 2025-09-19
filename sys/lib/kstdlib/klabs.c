#include <lib/kstdlib/klabs.h>
#include <lib/kstdlib/kstdlib.h>

long
    labs (long a) {
	return a > 0 ? a : -a;
}

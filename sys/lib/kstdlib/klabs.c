#include <klabs.h>
#include <kstdlib.h>

long
    labs (long a) {
	return a > 0 ? a : -a;
}

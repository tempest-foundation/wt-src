#include <lib/kmath/kisnan.h>
#include <lib/kmath/kmath.h>

int
    kisnan (double x) {
	return x != x;
}

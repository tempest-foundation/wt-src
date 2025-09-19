#include <lib/kmath/kisfinite.h>
#include <lib/kmath/kmath.h>

int
    kisfinite (double x) {
	return !kisnan(x) && !kisinf(x);
}

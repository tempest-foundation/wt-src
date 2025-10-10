#include <kisfinite.h>
#include <kmath.h>

int
    kisfinite (double x) {
	return !kisnan(x) && !kisinf(x);
}

#include <lib/kmath/kfmin.h>
#include <lib/kmath/kmath.h>

double
    kfmin (double x, double y) {
	// Handle NaN cases
	if (x != x)
		return y;
	if (y != y)
		return x;

	return x < y ? x : y;
}

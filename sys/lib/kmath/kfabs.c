#include <lib/kmath/kfabs.h>
#include <lib/kmath/kmath.h>

double
    kfabs (double x) {
	return x < 0.0 ? -x : x;
}

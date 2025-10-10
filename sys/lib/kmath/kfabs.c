#include <kfabs.h>
#include <kmath.h>

double
    kfabs (double x) {
	return x < 0.0 ? -x : x;
}

#include <kisinf.h>
#include <kmath.h>

int
    kisinf (double x) {
	return x == 1.0 / 0.0 || x == -1.0 / 0.0;
}

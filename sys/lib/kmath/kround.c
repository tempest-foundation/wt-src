#include <kmath.h>
#include <kround.h>

double
    kround (double x) {
	if (x >= 0.0) {
		return kfloor(x + 0.5);
	} else {
		return kceil(x - 0.5);
	}
}

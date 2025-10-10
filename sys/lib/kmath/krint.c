#include <kmath.h>
#include <krint.h>

double
    krint (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (kisinf(x))
		return x;

	// Round to nearest integer
	if (x >= 0.0) {
		double frac = x - kfloor(x);
		if (frac < 0.5)
			return kfloor(x);
		else
			return kceil(x);
	} else {
		double frac = kceil(x) - x;
		if (frac < 0.5)
			return kceil(x);
		else
			return kfloor(x);
	}
}

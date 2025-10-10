#include <kmath.h>
#include <ktrunc.h>

double
    ktrunc (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (kisinf(x))
		return x;

	// For positive numbers, truncate towards zero
	if (x >= 0.0)
		return kfloor(x);
	else
		return kceil(x);
}

#include <kmath.h>
#include <kremainder.h>

double
    kremainder (double x, double y) {
	// Handle special cases
	if (kisnan(x) || kisnan(y))
		return knan("");
	if (kisinf(x))
		return knan("");
	if (y == 0.0)
		return knan("");
	if (kisinf(y))
		return x;

	// Calculate remainder using fmod and adjust for rounding
	double r = kfmod(x, y);

	// Adjust to get remainder in the range [-|y|/2, |y|/2]
	if (kfabs(r) > kfabs(y) / 2.0) {
		if (r > 0)
			r -= kfabs(y);
		else
			r += kfabs(y);
	}

	return r;
}

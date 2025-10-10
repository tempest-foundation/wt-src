#include <khypot.h>
#include <kmath.h>

double
    khypot (double x, double y) {
	// Handle special cases
	if (kisinf(x) || kisinf(y)) {
		return 1.0 / 0.0;  // Return infinity
	}

	if (kisnan(x) || kisnan(y)) {
		return 0.0;  // Return NaN
	}

	// Use the formula: ksqrt(x^2 + y^2)
	return ksqrt(x * x + y * y);
}

#include <kfrexp.h>
#include <kmath.h>

double
    kfrexp (double x, int *exp) {
	// Handle special cases
	if (kisnan(x)) {
		*exp = 0;
		return knan("");
	}
	if (kisinf(x)) {
		*exp = 0;
		return x;
	}
	if (x == 0.0) {
		*exp = 0;
		return 0.0;
	}

	// Extract exponent using log2
	*exp = (int) kfloor(klog2(kfabs(x))) + 1;

	// Calculate mantissa: x / 2^exp
	return x / kpow(2.0, *exp);
}

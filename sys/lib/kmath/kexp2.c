#include <lib/kmath/kexp2.h>
#include <lib/kmath/kmath.h>

double
    kexp2 (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (kisinf(x))
		return x > 0 ? K_INFINITY : 0.0;

	// Use the relationship: exp2(x) = exp(x * ln(2))
	return kexp(x * K_M_LN2);
}

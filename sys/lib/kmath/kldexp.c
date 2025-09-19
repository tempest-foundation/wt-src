#include <lib/kmath/kldexp.h>
#include <lib/kmath/kmath.h>

double
    kldexp (double x, int exp) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (kisinf(x))
		return x;
	if (x == 0.0)
		return 0.0;

	// Use the relationship: ldexp(x, exp) = x * 2^exp
	return x * kpow(2.0, exp);
}

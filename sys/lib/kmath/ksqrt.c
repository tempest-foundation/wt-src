#include <kmath.h>
#include <ksqrt.h>

double
    ksqrt (double x) {
	if (x < 0.0) {
		// Return NaN for negative numbers
		return 0.0;
	}

	if (x == 0.0 || x == 1.0) {
		return x;
	}

	// Newton-Raphson method for square root
	double guess = x / 2.0;
	double prev_guess;

	do {
		prev_guess = guess;
		guess      = (guess + x / guess) / 2.0;
	} while (kfabs(guess - prev_guess) > 1e-15);

	return guess;
}

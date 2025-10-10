#include <klog.h>
#include <kmath.h>

double
    klog (double x) {
	// Handle special cases
	if (x <= 0.0) {
		// Return NaN for non-positive numbers
		return 0.0;
	}

	if (x == 1.0)
		return 0.0;

	// Use Newton's method to find ln(x)
	// We solve: e^y = x, so y = ln(x)
	double y = 0.0;

	// Initial guess
	if (x > 1.0) {
		y = 1.0;
	} else {
		y = -1.0;
	}

	// Newton iteration: y_{n+1} = y_n + (x - e^y_n) / e^y_n
	for (int i = 0; i < 10; i++) {
		double prev_y = y;
		double exp_y  = kexp(y);
		y             = y + (x - exp_y) / exp_y;

		if (kfabs(y - prev_y) < 1e-15) {
			break;
		}
	}

	return y;
}

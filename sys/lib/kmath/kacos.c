#include <kacos.h>
#include <kmath.h>

double
    kacos (double x) {
	// Check for invalid input
	if (x < -1.0 || x > 1.0)
		return knan("");

	// Use the relationship: acos(x) = π/2 - asin(x)
	return K_M_PI_2 - kasin(x);
}

#include <kceil.h>
#include <kmath.h>

double
    kceil (double x) {
	if (x <= 0.0) {
		// For negative numbers, truncate
		return (double) ((long long) x);
	} else {
		// For positive numbers, round up
		long long int_part = (long long) x;
		if (x == (double) int_part) {
			return x;
		}
		return (double) (int_part + 1);
	}
}

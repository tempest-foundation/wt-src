#include <kmath.h>
#include <ktan.h>

double
    ktan (double x) {
	double cos_x = kcos(x);
	if (cos_x == 0.0) {
		// Return infinity for undefined values
		return 1.0 / 0.0;
	}
	return ksin(x) / cos_x;
}

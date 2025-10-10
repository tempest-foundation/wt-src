#include <kcopysign.h>
#include <kmath.h>

double
    kcopysign (double x, double y) {
	if (y >= 0.0) {
		return kfabs(x);
	} else {
		return -kfabs(x);
	}
}

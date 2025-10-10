#include <kisnan.h>
#include <kmath.h>

int
    kisnan (double x) {
	return x != x;
}

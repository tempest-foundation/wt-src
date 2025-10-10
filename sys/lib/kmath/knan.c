#include <kmath.h>
#include <knan.h>

double
    knan (const char *tagp) {
	// Return NaN - the tagp parameter is ignored in this implementation
	(void) tagp;  // Suppress unused parameter warning

	// Return a quiet NaN
	union {
		double             d;
		unsigned long long u;
	} u;

	u.u = 0x7ff8000000000000ULL;  // Quiet NaN
	return u.d;
}

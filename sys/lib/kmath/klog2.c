#include <klog2.h>
#include <kmath.h>

double
    klog2 (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (x <= 0.0)
		return knan("");

	// Use the relationship: log2(x) = ln(x) / ln(2)
	return klog(x) / K_M_LN2;
}

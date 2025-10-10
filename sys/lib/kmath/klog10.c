#include <klog10.h>
#include <kmath.h>

double
    klog10 (double x) {
	// Handle special cases
	if (kisnan(x))
		return knan("");
	if (x <= 0.0)
		return knan("");

	// Use the relationship: log10(x) = ln(x) / ln(10)
	return klog(x) / klog(10);
}

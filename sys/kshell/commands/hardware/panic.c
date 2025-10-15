#include "panic.h"

#include <katoi.h>
#include <kprint.h>

#include <kern/panic/panic.h>

void
    cmd_panic (const char *args) {
	if (args == KNULL || *args == '\0') {
		kputs("Usage: kpanic <error_code>");
		kputs("Error codes: 0-16 (0=unknown, 1=div_by_zero, etc.)");
		return;
	}

	int code = katoi(args);
	if (code < 0 || code > 16) {
		kputs("Error code must be between 0 and 16");
		return;
	}

	panic.init(code, KNULL);
}

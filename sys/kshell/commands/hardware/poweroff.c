#include "poweroff.h"

#include <kern/acpi/acpi.h>

void
    cmd_poweroff (const char *args) {
	(void) args;
	acpi.poweroff();
}

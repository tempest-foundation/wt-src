#include <kern/acpi/acpi.h>

void
    cmd_reboot (const char *args) {
	(void) args;
	acpi.reboot();
}

#pragma once

extern struct Acpi acpi;

struct Acpi {
	void (*poweroff)(void);
	void (*reboot)(void);
};

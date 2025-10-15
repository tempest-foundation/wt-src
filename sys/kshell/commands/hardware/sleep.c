#include <katoi.h>
#include <kprint.h>
#include <ksleep.h>

void
    cmd_sleep (const char *args) {
	if (args && *args) {
		ksleep(katoi(args));
	} else {
		kputs("Zzzzzzzzzzzzzzzzzzz...");
		kputs("Use: sleep <time in milliseconds>");
	}
}

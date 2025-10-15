#include <kprint.h>
#include <ktime.h>

void
    cmd_time (const char *args) {
	(void) args;

	// Buffer for date and time strings
	char date_buffer[16];
	char time_buffer[16];

	// Retrieve date and time strings
	time.get_date_str(date_buffer, sizeof(date_buffer));
	time.get_time_str(time_buffer, sizeof(time_buffer));

	kprintf("Date: %s\n", date_buffer);
	kprintf("Time: %s\n", time_buffer);
}

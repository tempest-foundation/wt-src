#include <kstdio.h>

#include <kern/framebuf/framebuf.h>
#include <kern/memory/memory.h>

void
    cmd_fetch (const char *args) {
	(void) args;
	const char *ascii[] = {"@                         @",
	                       " @@           @       @@@@",
	                       "     @@@@@@@           ",
	                       "@                        @@",
	                       "   @@@@@@             @ ",
	                       "            @@@@@@@@     ",
	                       "@                         @",
	                       " @     @@@@@@@@@@      @@ "};
	char        info[8][96];
	ksnprintf(info[0], sizeof(info[0]), "os:     wind");
	ksnprintf(info[1], sizeof(info[1]), "kernel: tempest");
	extern char cpu_brand_string[49];
	ksnprintf(info[2], sizeof(info[2]), "cpu:    %s", cpu_brand_string);
	if (fb_info.width && fb_info.height && fb_info.bpp) {
		ksnprintf(info[3],
		          sizeof(info[3]),
		          "resolution: %ux%u %ubpp",
		          fb_info.width,
		          fb_info.height,
		          (unsigned int) fb_info.bpp);
	} else {
		ksnprintf(info[3], sizeof(info[3]), "resolution: unknown");
	}
	memory_stats_t stats    = memory_get_stats();
	kuint64_t      total_kb = stats.total_physical_pages * 4096 / 1024;
	kuint64_t      used_kb  = stats.used_physical_pages * 4096 / 1024;
	kuint64_t      free_kb  = stats.free_physical_pages * 4096 / 1024;
	kuint64_t      total_mb = total_kb / 1024;
	kuint64_t      used_mb  = used_kb / 1024;
	kuint64_t      free_mb  = free_kb / 1024;
	if (total_mb >= 1) {
		ksnprintf(info[4],
		          sizeof(info[4]),
		          "memory: %llu MB used / %llu MB total (%llu MB free)",
		          used_mb,
		          total_mb,
		          free_mb);
	} else {
		ksnprintf(info[4],
		          sizeof(info[4]),
		          "memory: %llu kB used / %llu kB total (%llu kB free)",
		          used_kb,
		          total_kb,
		          free_kb);
	}
	info[5][0] = '\0';
	info[6][0] = '\0';
	info[7][0] = '\0';

	for (int i = 0; i < 8; ++i) {
		if (info[i][0])
			kprintf("%-28s  %s\n", ascii[i], info[i]);
		else
			kprintf("%s\n", ascii[i]);
	}
}

#pragma once

#include <kstdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern struct Ata ata;

struct Ata {
	void (*io_wait)(void);
	int (*poll)(void);
	int (*pio_read)(kuint64_t lba, kuint32_t count, void *buf);
};

#ifdef __cplusplus
}
#endif

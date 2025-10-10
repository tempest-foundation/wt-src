#pragma once

#include <kstdint.h>

extern struct rand32 rand32;

struct rand32 {
	kint32_t (*sign)(void);
	kuint32_t (*uns)(void);
};

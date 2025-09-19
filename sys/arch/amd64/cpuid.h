#pragma once

#define CPU_BRAND_STRING_LEN 64
extern char cpu_brand_string[CPU_BRAND_STRING_LEN + 1];
void
    kcpu_init_brand (void);

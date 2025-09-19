#include "arch/amd64/kasm/kcpu_relax.h"

void
    kcpu_relax (void) {
	__asm__ volatile("pause");
}

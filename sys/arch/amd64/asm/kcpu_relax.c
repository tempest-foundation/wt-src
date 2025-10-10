#include "kcpu_relax.h"

void
    kcpu_relax (void) {
	__asm__ volatile("pause");
}

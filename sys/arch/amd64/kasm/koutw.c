#include "arch/amd64/kasm/koutw.h"

#include "arch/amd64/kasm/kio.h"

void
    koutw (unsigned short port, unsigned short val) {
	__asm__ __volatile__("outw %0, %1" : : "a"(val), "Nd"(port));
}

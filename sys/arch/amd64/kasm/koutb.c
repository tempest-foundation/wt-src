#include "arch/amd64/kasm/koutb.h"

#include "arch/amd64/kasm/kio.h"

void
    koutb (unsigned short port, unsigned char val) {
	__asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

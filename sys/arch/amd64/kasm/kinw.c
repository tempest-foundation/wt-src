#include "arch/amd64/kasm/kinw.h"

kuint16_t
    kinw (kuint16_t port) {
	kuint16_t val;
	__asm__ __volatile__("inw %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

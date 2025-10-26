#include <kstdint.h>

namespace amd64::sse2 {
	void enable(void) {
		// Enable SSE 2
		uint64_t cr4;
		__asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
		cr4 |= (3 << 9);  // Set OSFXSR and OSXMMEXCPT bits
		__asm__ volatile("mov %0, %%cr4" : : "r"(cr4));
	}
}  // namespace amd64::sse2

#include "halt.h"

namespace amd64 {
	void halt(void) {
// Good night, computer!
#ifdef ARCH_AMD64
		__asm__("cli; hlt");
#endif
	}
}  // namespace amd64

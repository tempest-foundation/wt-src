#include <kstdint.h>

#include <dbg/logger.h>
#include <kern/syscall/syscall.h>

kuint64_t
    sys_write (kuint64_t syscall_no __attribute__((unused)),
               kuint64_t fd,
               kuint64_t buffer,
               kuint64_t count,
               kuint64_t arg3 __attribute__((unused)),
               kuint64_t arg4 __attribute__((unused)),
               kuint64_t arg5 __attribute__((unused))) {
	logger.printf("syscall",
	              "info",
	              "write: fd=%llu, buffer=0x%llx, count=%llu\n",
	              fd,
	              buffer,
	              count);
	// TODO: Implement actual file writing
	// Basic validation
	if (buffer == 0 || count == 0) {
		return SYSCALL_INVALID_ARGS;
	}

	// For stdout (fd=1), we could implement a simple write to console
	if (fd == 1) {
		// TODO: Copy data from user buffer and write to console
		logger.printf("syscall", "info", "Write to stdout requested\n");
		return count;  // Pretend we wrote all bytes
	}

	return SYSCALL_NOT_IMPLEMENTED;
}

#include <kstdint.h>

#include <dbg/logger.h>
#include <kern/syscall/syscall.h>

kuint64_t
    sys_read (kuint64_t syscall_no __attribute__((unused)),
              kuint64_t fd,
              kuint64_t buffer,
              kuint64_t count,
              kuint64_t arg3 __attribute__((unused)),
              kuint64_t arg4 __attribute__((unused)),
              kuint64_t arg5 __attribute__((unused))) {
	logger.printf("syscall",
	              "info",
	              " read: fd=%llu, buffer=0x%llx, count=%llu\n",
	              fd,
	              buffer,
	              count);
	// TODO: Implement actual file reading
	// Basic validation
	if (buffer == 0 || count == 0) {
		return SYSCALL_INVALID_ARGS;
	}
	return SYSCALL_NOT_IMPLEMENTED;
}

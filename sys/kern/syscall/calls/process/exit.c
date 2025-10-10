#include <kstdint.h>

#include <dbg/logger.h>
#include <kern/syscall/syscall.h>

kuint64_t
    sys_exit (kuint64_t syscall_no __attribute__((unused)),
              kuint64_t status,
              kuint64_t arg1 __attribute__((unused)),
              kuint64_t arg2 __attribute__((unused)),
              kuint64_t arg3 __attribute__((unused)),
              kuint64_t arg4 __attribute__((unused)),
              kuint64_t arg5 __attribute__((unused))) {
	logger.printf("syscall", "info", "Process exit with status %llu\n", status);
	// TODO: Implement actual process termination
	return SYSCALL_NOT_IMPLEMENTED;
}

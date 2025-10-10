#include <kstdint.h>

#include <kern/syscall/syscall.h>

kuint64_t
    sys_getpid (kuint64_t syscall_no __attribute__((unused)),
                kuint64_t arg0 __attribute__((unused)),
                kuint64_t arg1 __attribute__((unused)),
                kuint64_t arg2 __attribute__((unused)),
                kuint64_t arg3 __attribute__((unused)),
                kuint64_t arg4 __attribute__((unused)),
                kuint64_t arg5 __attribute__((unused)));

#pragma once

#include <lib/kstdarg.h>
#include <lib/kstdio/kstddef.h>
#include <lib/kstdio/kstdint.h>

void
    kputhex (kuint64_t n);
void
    kputdec (kuint32_t n);
void
    kputs (const char *s);
int
    kvsnprintf (char *buffer, ksize_t size, const char *format, va_list args);
int
    ksnprintf (char *buffer, ksize_t size, const char *format, ...);
int
    kprintf (const char *format, ...);
void
    kputchar (int c);

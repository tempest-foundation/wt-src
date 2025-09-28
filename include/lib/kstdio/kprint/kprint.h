// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : include/lib/kstdio/kprint/kprint.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
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

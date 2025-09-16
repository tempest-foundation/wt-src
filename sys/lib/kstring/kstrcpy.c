// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/lib/kstring/kstrcpy.c
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include <lib/kstdio/kstrlen/kstrlen.h>
#include <lib/kstring/kmemcpy.h>
#include <lib/kstring/kstrcpy.h>

char *
    kstrcpy (char *dest, const char *src) {
	ksize_t len = kstrlen(src);
	kmemcpy(dest, src, len + 1);  // +1 for null terminator
	return dest;
}

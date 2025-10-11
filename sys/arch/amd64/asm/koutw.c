// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/arch/amd64/asm/koutw.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "koutw.h"

#include "kio.h"

void
    koutw (unsigned short port, unsigned short val) {
	__asm__ __volatile__("outw %0, %1" : : "a"(val), "Nd"(port));
}

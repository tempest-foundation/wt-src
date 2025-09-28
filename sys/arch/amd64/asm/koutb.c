// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/arch/amd64/asm/koutb.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "arch/amd64/asm/koutb.h"

#include "arch/amd64/asm/kio.h"

void
    koutb (unsigned short port, unsigned char val) {
	__asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

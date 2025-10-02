// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/arch/amd64/asm/kinw.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include "kinw.h"

kuint16_t
    kinw (kuint16_t port) {
	kuint16_t val;
	__asm__ __volatile__("inw %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/arch/amd64/kasm/kinw.c
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
#include "arch/amd64/kasm/kinw.h"

kuint16_t
    kinw (kuint16_t port) {
	kuint16_t val;
	__asm__ __volatile__("inw %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

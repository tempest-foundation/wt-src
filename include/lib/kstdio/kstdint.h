// SPDX-License-Identifier: GPL-3-0-only
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : include/lib/kstdio/kstdint.h
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#pragma once

// Int
typedef signed char kint8_t;
typedef short       kint16_t;
typedef int         kint32_t;
typedef long long   kint64_t;

// Unsigned Int
typedef unsigned char      kuint8_t;
typedef unsigned short     kuint16_t;
typedef unsigned int       kuint32_t;
typedef unsigned long long kuint64_t;

// Native sizes
typedef long long          kintptr_t;
typedef unsigned long long kuintptr_t;

// Maximum value for kuintptr_t
#define KUINTPTR_MAX 18446744073709551615ULL

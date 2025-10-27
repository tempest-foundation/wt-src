// SPDX-License-Identifier: GPL-3.0-only
/*
 * -- BEGIN METADATA HEADER --
 * <*---The Wind/Tempest Project---*>
 * 
 * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * 
 * Copyright (c) Tempest Foundation, 2025
 * -- END OF METADATA HEADER --
 */
#pragma once

#include "kacos.hpp"
#include "kasin.hpp"
#include "katan.hpp"
#include "katan2.hpp"
#include "kcbrt.hpp"
#include "kceil.hpp"
#include "kcopysign.hpp"
#include "kcos.hpp"
#include "kexp.hpp"
#include "kexp2.hpp"
#include "kexpm1.hpp"
#include "kfabs.hpp"
#include "kfloor.hpp"
#include "kfmax.hpp"
#include "kfmin.hpp"
#include "kfmod.hpp"
#include "kfrexp.hpp"
#include "khypot.hpp"
#include "kisfinite.hpp"
#include "kisinf.hpp"
#include "kisnan.hpp"
#include "kldexp.hpp"
#include "klog.hpp"
#include "klog10.hpp"
#include "klog1p.hpp"
#include "klog2.hpp"
#include "kmath.hpp"
#include "kmodf.hpp"
#include "knan.hpp"
#include "knearbyint.hpp"
#include "knextafter.hpp"
#include "kpow.hpp"
#include "kremainder.hpp"
#include "kremquo.hpp"
#include "krint.hpp"
#include "kround.hpp"
#include "ksin.hpp"
#include "ksqrt.hpp"
#include "ktan.hpp"
#include "ktrunc.hpp"

// Mathematical constants
#define K_M_E        2.7182818284590452354   // e
#define K_M_LOG2E    1.4426950408889634074   // log_2 e
#define K_M_LOG10E   0.43429448190325182765  // log_10 e
#define K_M_LN2      0.69314718055994530942  // log_e 2
#define K_M_LN10     2.30258509299404568402  // log_e 10
#define K_M_PI       3.14159265358979323846  // pi
#define K_M_PI_2     1.57079632679489661923  // pi/2
#define K_M_PI_4     0.78539816339744830962  // pi/4
#define K_M_1_PI     0.31830988618379067154  // 1/pi
#define K_M_2_PI     0.63661977236758134308  // 2/pi
#define K_M_2_SQRTPI 1.12837916709551257390  // 2/sqrt(pi)
#define K_M_SQRT2    1.41421356237309504880  // sqrt(2)
#define K_M_SQRT1_2  0.70710678118654752440  // 1/sqrt(2)

/*
 ! Just a fucking warning!
 * FP_INFINITE: classification macro (integer)
 * INFINITY: floating-point constant (IEEE 754 positive infinity)
 */

// Infinity and NaN constants
#define K_INFINITY (1.0 / 0.0)
#define K_NAN      (0.0 / 0.0)

// Classification macros
#define K_FP_NAN       0
#define K_FP_ZERO      2
#define K_FP_SUBNORMAL 3
#define K_FP_NORMAL    4
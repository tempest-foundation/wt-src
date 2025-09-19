#pragma once

#ifdef _STDINT_H
#	error \
	    "Do NOT include <stdint.h> before kstdint.h! Use only kstdint.h in this project."
#endif

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

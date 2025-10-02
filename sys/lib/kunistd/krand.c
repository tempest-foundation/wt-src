// SPDX-License-Identifier: LSL-2.0
/*
 * -- BEGIN METADATA HEADER --
 * The Wind/Tempest Project
 *
 * File       : sys/lib/kunistd/krand.c
 * Author     : Tempik25 <tempik25@tempestfoundation.org>
 * Maintainer : Tempest Foundation <development@tempestfoundation.org>
 * Repo       : https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * -- END OF METADATA HEADER --
 */
#include <krand.h>
#include <kstdbool.h>
#include <kstdint.h>

#define PCG32_MULT     6364136223846793005ULL
#define PCG32_INIT_SEQ 0xDEADBEEFULL

static kuint64_t pcg_state  = 0;
static kuint64_t pcg_inc    = 0;
static kbool     pcg_inited = kfalse;

static inline kuint32_t
    rdtsc32 (void) {
	kuint32_t lo;
	__asm__ volatile("rdtsc" : "=a"(lo)::"edx");
	return lo;
}

static inline kuint32_t
    pcg32_output (kuint64_t prev_state) {
	// Output function (XSH RR), as per PCG reference
	kuint32_t xorshifted = (kuint32_t) (((prev_state >> 18u) ^ prev_state) >> 27u);
	kuint32_t rot        = (kuint32_t) (prev_state >> 59u);
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void
    srand32 (kuint64_t seed, kuint64_t seq) {
	pcg_state = 0;
	pcg_inc   = (seq << 1) | 1;
	// Advance the state a few times to mix up bits (PCG recommends at least once)
	for (int i = 0; i < 3; i++) {
		pcg_state = pcg_state * PCG32_MULT + pcg_inc;
	}
	pcg_state += seed;
	pcg_state  = pcg_state * PCG32_MULT + pcg_inc;
	pcg_inited = ktrue;
}

static void
    ensure_pcg_init (void) {
	if (!pcg_inited) {
		kuint32_t t = rdtsc32();
		srand32((kuint64_t) t, (kuint64_t) (t ^ PCG32_INIT_SEQ));
	}
}

kuint32_t
    rand32_u (void) {
	ensure_pcg_init();
	kuint64_t prev_state = pcg_state;
	pcg_state            = prev_state * PCG32_MULT + pcg_inc;
	return pcg32_output(prev_state);
}

kint32_t
    rand32_s (void) {
	return (kint32_t) rand32_u();
}

struct rand32 rand32 = {
    .sign = rand32_s,
    .uns  = rand32_u,
};

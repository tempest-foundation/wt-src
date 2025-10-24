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
#include <krand.h>
#include <kstdint.h>

#define PCG32_MULT     6364136223846793005ULL
#define PCG32_INIT_SEQ 0xDEADBEEFULL

static uint64_t pcg_state  = 0;
static uint64_t pcg_inc    = 0;
static bool     pcg_inited = false;

namespace unistd {
	namespace rand {

		/**
 * @brief Read the CPU timestamp counter (lower 32 bits)
 *
 * Uses the RDTSC instruction to obtain a relatively random,
 * time-based seed for entropy when initializing PRNG state.
 *
 * @return Lower 32 bits of the CPU timestamp counter
 */
		static inline uint32_t rdtsc32(void) {
			uint32_t lo;
			__asm__ volatile("rdtsc" : "=a"(lo)::"edx");
			return lo;
		}

		/**
 * @brief PCG32 output function (XSH RR), as per PCG reference
 *
 * Takes the previous state and produces a 32-bit pseudo-random number
 *
 * @param prev_state Previous PRNG state
 * @return 32-bit unsigned random number derived from the given state
 */
		static inline uint32_t pcg32_output(uint64_t prev_state) {
			uint32_t xorshifted =
			    (uint32_t) (((prev_state >> 18u) ^ prev_state) >> 27u);
			uint32_t rot = (uint32_t) (prev_state >> 59u);
			return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
		}

		/**
 * @brief Seed the PCG32 pseudo-random number generator
 *
 * The seed and stream selector (seq) are used to initialize
 * the state and increment value of the PCG32 generator.
 *
 * @param seed 64-bit seed value
 * @param seq Per-stream sequence selector (stream id)
 */
		void srand32(uint64_t seed, uint64_t seq) {
			pcg_state = 0;
			pcg_inc   = (seq << 1) | 1;
			// Advance the state a few times to mix up bits (PCG recommends at least once)
			for( int i = 0; i < 3; i++ ) {
				pcg_state = pcg_state * PCG32_MULT + pcg_inc;
			}
			pcg_state += seed;
			pcg_state  = pcg_state * PCG32_MULT + pcg_inc;
			pcg_inited = true;
		}

		/**
 * @brief Ensure PCG32 is initialized; seed from timestamp if not
 *
 * If the PRNG has not yet been explicitly seeded, this will
 * use the current timestamp counter as entropy.
 */
		static void ensure_pcg_init(void) {
			if( !pcg_inited ) {
				uint32_t t = rdtsc32();
				srand32((uint64_t) t, (uint64_t) (t ^ PCG32_INIT_SEQ));
			}
		}

		/**
 * @brief Get a random unsigned 32-bit integer
 *
 * Generates a new random number from the PCG32 state.
 * Automatically initializes the generator if needed.
 *
 * @return A random 32-bit unsigned integer
 */
		uint32_t unsign(void) {
			ensure_pcg_init();
			uint64_t prev_state = pcg_state;
			pcg_state           = prev_state * PCG32_MULT + pcg_inc;
			return pcg32_output(prev_state);
		}

		/**
 * @brief Get a random signed 32-bit integer
 *
 * Uses rand32_u() and casts the result to signed.
 *
 * @return A random 32-bit signed integer
 */
		int32_t sign(void) {
			return (int32_t) unsign();
		}
	}  // namespace rand
}  // namespace unistd

/**
 * @file round_up_pow2_32.h
 * @brief Round up to the next power of two
 *
 * Sources used:
 *   @li Fallback: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
 *   @li Intrinsics: https://en.wikipedia.org/wiki/Find_first_set#Tool_and_library_support
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Round up to the next power of two (fallback).
 *
 * Assumes:
 * @li `x` is strictly larger than 0.
 * @li `x` is smaller than than or equal to UINT32_MAX / 2 + 1.
 *
 * @param x                     The number at hand.
 *
 * @return                      A power of two that is larger than or equal to the given number.
 */
static inline uint32_t round_up_pow2_32_fallback(uint32_t x)
{
    assert(0 < x && x <= UINT32_MAX / 2 + 1);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

/**
 * Round up to the next power of two.
 *
 * Assumes:
 * @li `x` is strictly larger than 0.
 * @li `x` is smaller than than or equal to UINT32_MAX / 2 + 1.
 *
 * @param x                     The number at hand.
 *
 * @return                      A power of two that is larger than or equal to the given number.
 */
static inline uint32_t round_up_pow2_32(uint32_t x)
{
    assert(0 < x && x <= UINT32_MAX / 2 + 1);

// Test for GCC >= 3.4.0
#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && (__GNUC_MINOR__ > 4 || __GNUC_MINOR__ == 4)))
    return x == 1U ? 1U : 1U << (32 - __builtin_clz(x - 1U));
#else
    return round_up_pow2_32_fallback(x);
#endif
}

#ifdef __cplusplus
}
#endif

// vim: ft=c

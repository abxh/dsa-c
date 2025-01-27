/*  murmurhash.h
 *
 *  Copyright (C) 2025 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information. */

/**
 * @file murmurhash.h
 * @brief Murmur3 hash hashing function
 *
 * @note Murmur3 hash is **not** a cryptographic hashing function.
 *
 * Original Source:
 * http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp
 *
 * Source used:
 * https://en.wikipedia.org/wiki/MurmurHash#Algorithm
 *
 * MurmurHash3 was written by Austin Appleby, and is placed in the public
 * domain.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/// @cond DO_NOT_DOCUMENT
static inline uint32_t internal_murmur_32_scramble(uint32_t k)
{
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}
/// @endcond

/**
 * @brief Get the Murmur3 (32-bit) hash of a string of bytes.
 *
 * @param[in] key_ptr           Pointer to the string of bytes.
 * @param[in] len               Number of bytes.
 * @param[in] seed              A seed, for whom matched with a given key, makes the
 *                              hash function produce the same hash for the key.
 *
 * @return                      A `uint32_t`-sized hash of the bytes.
 */
static inline uint32_t murmur3_32(const uint8_t *key_ptr, const uint32_t len, const uint32_t seed)
{
    uint32_t h = seed;
    uint32_t k;

    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        memcpy(&k, key_ptr, sizeof(uint32_t));
        key_ptr += sizeof(uint32_t);
        h ^= internal_murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }

    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key_ptr[i - 1];
    }

    // A swap is *not* necessary here because the preceding loop already
    // places the low bytes in the low places according to whatever
    // endianness we use. Swaps only apply when the memory is copied in a
    // chunk.
    h ^= internal_murmur_32_scramble(k);

    /* Finalize. */
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

#ifdef __cplusplus
}
#endif

// vim: ft=c

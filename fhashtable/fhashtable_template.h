/*  fhashtable_template.h
 *
 *  Copyright (C) 2023 abxh
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information. */

/**
 * @file fhashtable_template.h
 * @brief Fixed-size open-adressing hashtable (robin hood hashing)
 *
 * Ensure the capacity rounded up to the power of 2 is 75% of the expected
 * numbers of values to be stored to keep load factor low and the hash table
 * performant.
 *
 * Prefer to use scalar types (int/uint/pointers) or strings as key/value pairs.
 * Structs can be used with elementwise equality check but will not make use the
 * cache and hardware prefetching as well. Keep the structs in a seperate buffer
 * and use their pointers preferably.
 *
 * A static / heap-allocated buffer should be used, should the key/values's
 * lifetime extend beyond the scope the arguments are provided. This applies to
 * strings. See example.
 *
 * The following macros must be defined:
 *      @li `NAME`
 *      @li `KEY_TYPE`
 *      @li `VALUE_TYPE`
 *      @li `KEY_IS_EQUAL(a,b)`
 *      @li `HASH_FUNCTION(key)`
 *
 * Source(s) used:
 *  @li https://thenumb.at/Hashtables/#robin-hood-linear-probing
 *  @li https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/
 */

#include "fnvhash.h"          // fnvhash_32, fnvhash_32_str
#include "murmurhash.h"       // murmur_32
#include "round_up_pow2_32.h" // round_up_pow2_32

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// macro definitions: {{{

/**
 * @def PASTE(a,b)
 * @brief Paste two tokens together.
 */
#ifndef PASTE
#define PASTE(a, b) a##b
#endif

/**
 * @def XPASTE(a,b)
 * @brief First expand tokens, then paste them together.
 */
#ifndef XPASTE
#define XPASTE(a, b) PASTE(a, b)
#endif

/**
 * @def JOIN(a,b)
 * @brief First expand tokens, then paste them together with a _ in between.
 */
#ifndef JOIN
#define JOIN(a, b) XPASTE(a, XPASTE(_, b))
#endif

/**
 * @brief Macro to check if a number is a power of two.
 *
 * @param[in] x             The number at hand.
 *
 * @return                  A boolean value indicating whether the number is a power of two.
 */
#ifndef IS_POW2
#define IS_POW2(X) ((X) != 0 && ((X) & ((X) - 1)) == 0)
#endif

/**
 * @def FHASHTABLE_EMPTY_SLOT_OFFSET
 * @brief Offset constant used to flag empty slots.
 */
#ifndef FHASHTABLE_EMPTY_SLOT_OFFSET
#define FHASHTABLE_EMPTY_SLOT_OFFSET (UINT32_MAX)
#endif

/**
 * @def FHASHTABLE_FOR_EACH(self, index, key_, value_)
 *
 * @brief Iterate over the non-empty slots in the hashtable in arbitary order.
 *
 * @warning Modifying the hashtable under the iteration may result in errors.
 *
 * @param[in] self              Hashtable pointer.
 * @param[in] index             Temporary indexing variable. Should be `uint32_t`.
 * @param[out] key_             Current key. Should be `KEY_TYPE`.
 * @param[out] value_           Current value. Should be `VALUE_TYPE`.
 */
#ifndef FHASHTABLE_FOR_EACH
#define FHASHTABLE_FOR_EACH(self, index, key_, value_)                    \
    for ((index) = 0; (index) < (self)->capacity; (index)++)              \
        if ((self)->slots[(index)].offset != FHASHTABLE_EMPTY_SLOT_OFFSET \
            && ((key_) = (self)->slots[(index)].key, (value_) = (self)->slots[(index)].value, true))
#endif

/**
 * @def FHASHTABLE_CALC_SIZEOF(fhashtable_name, capacity)
 *
 * @brief Calculate the size of the hashtable struct. No overflow checks.
 *
 * @param[in] fhashtable_name   Defined hashtable NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      The equivalent size.
 */
#ifndef FHASHTABLE_CALC_SIZEOF
#define FHASHTABLE_CALC_SIZEOF(fhashtable_name, capacity) \
    (uint32_t)(offsetof(struct fhashtable_name, slots) + capacity * sizeof(((struct fhashtable_name *)0)->slots[0]))
#endif

/**
 * @def FHASHTABLE_CALC_SIZEOF_OVERFLOWS(fhashtable_name, capacity)
 *
 * @brief Check for a given capacity, if the equivalent size of the hashtable struct overflows.
 *
 * @param[in] fhashtable_name   Defined hashtable NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      Whether the equivalent size overflows.
 */
#ifndef FHASHTABLE_CALC_SIZEOF_OVERFLOWS
#define FHASHTABLE_CALC_SIZEOF_OVERFLOWS(fhashtable_name, capacity) \
    (capacity                                                       \
     > (UINT32_MAX - offsetof(struct fhashtable_name, slots)) / sizeof(((struct fhashtable_name *)0)->slots[0]))
#endif

/**
 * @def NAME
 * @brief Prefix to hashtable types and operations. This must be manually
 *        defined before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#define FHASHTABLE_NAME fhashtable
#error "Must define NAME."
#else
#define FHASHTABLE_NAME NAME
#endif

/**
 * @def KEY_TYPE
 * @brief The key type. This must be manually defined before including this
 *        header file.
 *
 * Is undefined once header is included.
 */
#ifndef KEY_TYPE
#define KEY_TYPE int
#define FUNCTION_DEFINITIONS
#define TYPE_DEFINITIONS
#error "Must define KEY_TYPE."
#endif

/**
 * @def VALUE_TYPE
 * @brief The value type. This must be manually defined before
 *        including this header file.
 *
 * Is undefined once header is included.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#error "Must define VALUE_TYPE."
#endif

/**
 * @def KEY_IS_EQUAL(a, b)
 * @brief Used to compare two keys This must be manually defined before
 *        including this header file.
 *
 * Is undefined once header is included.
 *
 * @attention
 *   @li If comparing two scalar values, set this macro to ((a) == (b)).
 *   @li If comparing two strings, set this macro to strcmp() or strncmp()
 *       appropiately.
 *   @li If comparing two structs, set this macro to a function that does
 *       element-wise comparison between the structs.
 *
 * @retval true If the two keys are equal. Equivalent to a non-zero int.
 * @retval false If the two key are not equal. Equivalent to the int 0.
 */
#ifndef KEY_IS_EQUAL
#error "Must define KEY_IS_EQUAL."
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#endif

/**
 * @def HASH_FUNCTION(key)
 * @brief Used to compute indicies of keys. This must be manually defined before
 *        including this header file.
 *
 * Is undefined once header is included.
 *
 * @param key The key.
 * @return The hash of the key as `uint32_t`.
 */
#ifndef HASH_FUNCTION
#error "Must define HASH_FUNCTION."
#define HASH_FUNCTION(key) (murmur3_32((uint8_t *)&(key), sizeof(KEY_TYPE), 0))
#endif

/**
 * @def FUNCTION_LINKAGE
 * @brief Specify function linkage e.g. FUNCTION_LINKAGE
 */
#ifndef FUNCTION_LINKAGE
#define FUNCTION_LINKAGE
#endif

/// @cond DO_NOT_DOCUMENT
#define FHASHTABLE_TYPE         struct FHASHTABLE_NAME
#define FHASHTABLE_SLOT_TYPE    struct JOIN(FHASHTABLE_NAME, slot)
#define FHASHTABLE_SLOT         JOIN(FHASHTABLE_NAME, slot)
#define FHASHTABLE_INIT         JOIN(FHASHTABLE_NAME, init)
#define FHASHTABLE_IS_FULL      JOIN(FHASHTABLE_NAME, is_full)
#define FHASHTABLE_CONTAINS_KEY JOIN(FHASHTABLE_NAME, contains_key)
#define FHASHTABLE_SWAP_SLOTS   JOIN(internal, JOIN(FHASHTABLE_NAME, swap_slots))
#define FHASHTABLE_BACKSHIFT    JOIN(internal, JOIN(FHASHTABLE_NAME, backshift))
/// @endcond

// }}}

// type definitions: {{{

/**
 * @def TYPE_DEFINITIONS
 * @brief Define the types
 */
#ifdef TYPE_DEFINITIONS

/**
 * @brief Generated hashtable slot struct type for a given `KEY_TYPE` and
 *        `VALUE_TYPE`.
 */
struct JOIN(FHASHTABLE_NAME, slot) {
    uint32_t offset;  ///< Offset from the ideal slot index.
    KEY_TYPE key;     ///< The key in this slot
    VALUE_TYPE value; ///< The value in this slot
};

/**
 * @brief Generated hashtable struct type for a given `KEY_TYPE` and
 *        `VALUE_TYPE`.
 */
struct FHASHTABLE_NAME {
    uint32_t count;               ///< Number of non-empty slots.
    uint32_t capacity;            ///< Number of slots.
    FHASHTABLE_SLOT_TYPE slots[]; ///< Array of slots.
};

#endif

// }}}

// function declarations: {{{

/**
 * @brief Initialize a hashtable struct, given a (power-of-2) capacity.
 *
 * @param[in] self              Hashtable pointer
 * @param[in] pow2_capacity     Power of 2 capacity.
 */
FUNCTION_LINKAGE FHASHTABLE_TYPE *JOIN(FHASHTABLE_NAME, init)(FHASHTABLE_TYPE *self, const uint32_t pow2_capacity);

/**
 * @brief Create an hashtable with a given capacity with malloc().
 *
 * @param[in] min_capacity      Maximum number of elements to be stored.
 *
 * @return                      A pointer to the queue.
 * @retval NULL
 *   @li                        If malloc fails.
 *   @li                        If capacity is equal to 0 or larger than UINT32_MAX / 2 + 1 or the equivalent size
 *                              overflows.
 */
FUNCTION_LINKAGE FHASHTABLE_TYPE *JOIN(FHASHTABLE_NAME, create)(const uint32_t min_capacity);

/**
 * @brief Destroy an hashtable struct and free the underlying memory with
 *        free().
 *
 * @warning May not be called twice in a row on the same object.
 *
 * @param[in] self              The hashtable pointer.
 */
FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, destroy)(FHASHTABLE_TYPE *self);

/**
 * @brief Return whether the hashtable is empty.
 *
 * @param[in] self              The hashtable pointer.
 *
 * @return                      Whether the hashtable is empty.
 */
FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, is_empty)(const FHASHTABLE_TYPE *self);

/**
 * @brief Return whether the hashtable is full.
 *
 * @param[in] self              The hashtable pointer.
 *
 * @return                      Whether the hashtable is full.
 */
FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, is_full)(const FHASHTABLE_TYPE *self);

/**
 * @brief Check if hashtable contains a key.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key.
 *
 * @return                      A boolean indicating whether the hashtable contains the given key.
 */
FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, contains_key)(const FHASHTABLE_TYPE *self, const KEY_TYPE key);

/**
 * @brief From a given key, get the pointer to the corresponding value in the
 *        hashtable.
 *
 * @note The returned pointer is **not** garanteed to point to the same value if
 *       the hashtable is modified.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key to search for.
 *
 * @return                      A pointer to the corresponding key.
 *  @retval NULL                If the hashtable did not contain the key.
 */
FUNCTION_LINKAGE VALUE_TYPE *JOIN(FHASHTABLE_NAME, get_value_mut)(FHASHTABLE_TYPE *self, const KEY_TYPE key);

/**
 * @brief From a given key, get the copy of the corresponding value in the
 *        hashtable.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key to search for.
 * @param[in] default_value     The default value returned if the hashtable did
 *                              not contain the key.
 *
 * @return                      The corresponding key.
 * @retval `default_value`      If the hashtable did not contain the key.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FHASHTABLE_NAME, get_value)(const FHASHTABLE_TYPE *self, const KEY_TYPE key,
                                                             VALUE_TYPE default_value);

/**
 * @brief Search a given key in the hashtable and get the pointer to the
 *        corresponding value.
 *
 * @note The returned pointer is **not** garanteed to point to the same value if
 *       the hashtable is modified.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key to search for.
 *
 * @return                      A pointer to the corresponding key.
 * @retval NULL                 If the hashtable did not contain the key.
 */
FUNCTION_LINKAGE VALUE_TYPE *JOIN(FHASHTABLE_NAME, search)(FHASHTABLE_TYPE *self, const KEY_TYPE key);

/**
 * @brief Insert a non-duplicate key and it's corresponding value inside the
 *        hashtable.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key.
 * @param[in] value             The value.
 */
FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, insert)(FHASHTABLE_TYPE *self, KEY_TYPE key, VALUE_TYPE value);

/**
 * @brief Update a key's corresponding value inside the hashtable. Allows
 *        duplicates.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key.
 * @param[in] value             The value.
 */
FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, update)(FHASHTABLE_TYPE *self, KEY_TYPE key, VALUE_TYPE value);

/**
 * @brief Delete a key and it's corresponding value from the hashtable.
 *
 * @param[in] self              The hashtable pointer.
 * @param[in] key               The key.
 *
 * @return A boolean indicating whether the key was previously contained in the
 *         hashtable.
 */
FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, delete)(FHASHTABLE_TYPE *self, const KEY_TYPE key);

/**
 * @brief Clear an existing hashtable and flag all slots as empty.
 *
 * @param[in] self              The pointer of the hashtable to clear.
 */
FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, clear)(FHASHTABLE_TYPE *self);

/**
 * @brief Copy the values from a source hashtable to a destination hashtable.
 *
 * @param[out] dest_ptr         The destination hashtable.
 * @param[in] src_ptr           The source hashtable.
 */
FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, copy)(FHASHTABLE_TYPE *restrict dest_ptr,
                                                  const FHASHTABLE_TYPE *restrict src_ptr);

// @}}}

// function definitions: {{{

/**
 * @def FUNCTION_DEFINITIONS
 * @brief Define the functions
 */
#ifdef FUNCTION_DEFINITIONS

FUNCTION_LINKAGE FHASHTABLE_TYPE *JOIN(FHASHTABLE_NAME, init)(FHASHTABLE_TYPE *self, const uint32_t pow2_capacity)
{
    assert(self);
    assert(IS_POW2(pow2_capacity));

    self->count = 0;
    self->capacity = pow2_capacity;

    for (uint32_t i = 0; i < self->capacity; i++) {
        self->slots[i].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;
    }

    return self;
}

FUNCTION_LINKAGE FHASHTABLE_TYPE *JOIN(FHASHTABLE_NAME, create)(const uint32_t min_capacity)
{
    if (min_capacity == 0 || min_capacity > UINT32_MAX / 2 + 1) {
        return NULL;
    }

    const uint32_t capacity = round_up_pow2_32(min_capacity);

    if (FHASHTABLE_CALC_SIZEOF_OVERFLOWS(FHASHTABLE_NAME, capacity)) {
        return NULL;
    }

    const uint32_t size = FHASHTABLE_CALC_SIZEOF(FHASHTABLE_NAME, capacity);

    FHASHTABLE_TYPE *self = (FHASHTABLE_TYPE *)calloc(1, size);

    if (!self) {
        return NULL;
    }

    FHASHTABLE_INIT(self, capacity);

    return self;
}

FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, destroy)(FHASHTABLE_TYPE *self)
{
    assert(self);

    free(self);
}

FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, is_empty)(const FHASHTABLE_TYPE *self)
{
    assert(self != NULL);

    return self->count == 0;
}

FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, is_full)(const FHASHTABLE_TYPE *self)
{
    assert(self != NULL);

    return self->count == self->capacity;
}

FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, contains_key)(const FHASHTABLE_TYPE *self, const KEY_TYPE key)
{
    assert(self != NULL);

    const uint32_t key_hash = HASH_FUNCTION(key);
    const uint32_t index_mask = self->capacity - 1;

    uint32_t index = key_hash & index_mask;
    uint32_t max_possible_offset = 0;

    while (true) {
        const bool not_empty = self->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        const bool below_max = max_possible_offset <= self->slots[index].offset;

        if (!(not_empty && below_max)) {
            break;
        }

        if (KEY_IS_EQUAL(self->slots[index].key, key)) {
            return true;
        }

        index++;
        index &= index_mask;
        max_possible_offset++;
    }
    return false;
}

FUNCTION_LINKAGE VALUE_TYPE *JOIN(FHASHTABLE_NAME, get_value_mut)(FHASHTABLE_TYPE *self, const KEY_TYPE key)
{
    assert(self != NULL);

    const uint32_t key_hash = HASH_FUNCTION(key);
    const uint32_t index_mask = self->capacity - 1;

    uint32_t index = key_hash & index_mask;
    uint32_t max_possible_offset = 0;

    while (true) {
        const bool not_empty = self->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        const bool below_max = max_possible_offset <= self->slots[index].offset;

        if (!(not_empty && below_max)) {
            break;
        }

        if (KEY_IS_EQUAL(self->slots[index].key, key)) {
            return &self->slots[index].value;
        }

        index++;
        index &= index_mask;
        max_possible_offset++;
    }
    return NULL;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FHASHTABLE_NAME, get_value)(const FHASHTABLE_TYPE *self, const KEY_TYPE key,
                                                             VALUE_TYPE default_value)
{
    assert(self != NULL);

    const uint32_t key_hash = HASH_FUNCTION(key);
    const uint32_t index_mask = self->capacity - 1;

    uint32_t index = key_hash & index_mask;
    uint32_t max_possible_offset = 0;

    while (true) {
        const bool not_empty = self->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        const bool below_max = max_possible_offset <= self->slots[index].offset;

        if (!(not_empty && below_max)) {
            break;
        }

        if (KEY_IS_EQUAL(self->slots[index].key, key)) {
            return self->slots[index].value;
        }

        index++;
        index &= index_mask;
        max_possible_offset++;
    }
    return default_value;
}

FUNCTION_LINKAGE VALUE_TYPE *JOIN(FHASHTABLE_NAME, search)(FHASHTABLE_TYPE *self, const KEY_TYPE key)
{
    return JOIN(FHASHTABLE_NAME, get_value_mut)(self, key);
}

/// @cond DO_NOT_DOCUMENT
static inline void JOIN(internal, JOIN(FHASHTABLE_NAME, swap_slots))(FHASHTABLE_SLOT_TYPE *a, FHASHTABLE_SLOT_TYPE *b)
{
    FHASHTABLE_SLOT_TYPE temp = *a;
    *a = *b;
    *b = temp;
}
/// @endcond

FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, insert)(FHASHTABLE_TYPE *self, KEY_TYPE key, VALUE_TYPE value)
{
    assert(self != NULL);
    assert(FHASHTABLE_CONTAINS_KEY(self, key) == false);

    const uint32_t index_mask = self->capacity - 1;
    const uint32_t key_hash = HASH_FUNCTION(key);

    uint32_t index = key_hash & index_mask;
    FHASHTABLE_SLOT_TYPE current_slot = {.offset = 0, .key = key, .value = value};

    while (true) {
        const bool not_empty = self->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        if (!not_empty) {
            break;
        }

        if (current_slot.offset > self->slots[index].offset) {
            FHASHTABLE_SWAP_SLOTS(&self->slots[index], &current_slot);
        }

        index++;
        index &= index_mask;
        current_slot.offset++;
    }
    self->slots[index] = current_slot;
    self->count++;
}

FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, update)(FHASHTABLE_TYPE *self, KEY_TYPE key, VALUE_TYPE value)
{
    assert(self != NULL);

    const uint32_t index_mask = self->capacity - 1;
    const uint32_t key_hash = HASH_FUNCTION(key);

    uint32_t index = key_hash & index_mask;
    FHASHTABLE_SLOT_TYPE current_slot = {.offset = 0, .key = key, .value = value};

    while (true) {
        const bool not_empty = self->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        if (!not_empty) {
            break;
        }

        const bool offset_is_same = current_slot.offset == self->slots[index].offset;

        const bool key_is_equal = KEY_IS_EQUAL(current_slot.key, self->slots[index].key);

        if (offset_is_same && key_is_equal) {
            self->slots[index].value = current_slot.value;
            return;
        }

        if (current_slot.offset > self->slots[index].offset) {
            FHASHTABLE_SWAP_SLOTS(&current_slot, &self->slots[index]);
        }

        index++;
        index &= index_mask;
        current_slot.offset++;
    }

    self->slots[index] = current_slot;
    self->count++;
}

/// @cond DO_NOT_DOCUMENT
static inline void JOIN(internal, JOIN(FHASHTABLE_NAME, backshift))(FHASHTABLE_TYPE *self, const uint32_t index_mask,
                                                                    uint32_t index)
{
    assert(self);

    uint32_t next_index = (index + 1) & index_mask;

    while (true) {
        const bool not_empty = self->slots[next_index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        const bool offset_is_non_zero = self->slots[next_index].offset > 0;

        if (!(not_empty && offset_is_non_zero)) {
            break;
        }

        self->slots[index] = self->slots[next_index];
        self->slots[index].offset--;

        self->slots[next_index].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;

        index = next_index;
        next_index = (index + 1) & index_mask;
    }
}
/// @endcond

FUNCTION_LINKAGE bool JOIN(FHASHTABLE_NAME, delete)(FHASHTABLE_TYPE *self, const KEY_TYPE key)
{
    assert(self != NULL);

    const uint32_t index_mask = self->capacity - 1;
    const uint32_t key_hash = HASH_FUNCTION(key);

    uint32_t index = key_hash & index_mask;
    uint32_t max_possible_offset = 0;

    while (true) {
        const bool not_empty = self->slots[index].offset != FHASHTABLE_EMPTY_SLOT_OFFSET;

        const bool below_max = max_possible_offset <= self->slots[index].offset;

        if (!(not_empty && below_max)) {
            break;
        }

        const bool key_is_equal = KEY_IS_EQUAL(key, self->slots[index].key);

        if (!key_is_equal) {
            index = (index + 1) & index_mask;
            max_possible_offset++;
            continue;
        }

        self->slots[index].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;
        self->count--;

        FHASHTABLE_BACKSHIFT(self, index_mask, index);

        return true;
    }
    return false;
}

FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, clear)(FHASHTABLE_TYPE *self)
{
    assert(self != NULL);

    for (uint32_t i = 0; i < self->capacity; i++) {
        self->slots[i].offset = FHASHTABLE_EMPTY_SLOT_OFFSET;
    }
    self->count = 0;
}

FUNCTION_LINKAGE void JOIN(FHASHTABLE_NAME, copy)(FHASHTABLE_TYPE *restrict dest_ptr,
                                                  const FHASHTABLE_TYPE *restrict src_ptr)
{
    assert(src_ptr != NULL);
    assert(dest_ptr != NULL);
    assert(src_ptr->capacity <= dest_ptr->capacity);
    assert(dest_ptr->count == 0);

    for (uint32_t i = 0; i < src_ptr->capacity; i++) {
        dest_ptr->slots[i] = src_ptr->slots[i];
    }

    dest_ptr->count = src_ptr->count;
}

#endif

// }}}

// macro undefs: {{{

#undef NAME
#undef KEY_TYPE
#undef VALUE_TYPE
#undef KEY_IS_EQUAL
#undef HASH_FUNCTION
#undef FUNCTION_LINKAGE
#undef FUNCTION_DEFINITIONS
#undef TYPE_DEFINITIONS

#undef FHASHTABLE_TYPE
#undef FHASHTABLE_SLOT_TYPE
#undef FHASHTABLE_INIT
#undef FHASHTABLE_IS_FULL
#undef FHASHTABLE_CONTAINS_KEY
#undef FHASHTABLE_CALC_SIZEOF
#undef FHASHTABLE_SWAP_SLOTS
#undef FHASHTABLE_BACKSHIFT

// }}}

// vim: ft=c fdm=marker

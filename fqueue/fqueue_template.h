/*  fqueue_template.h
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
 * @file fqueue_template.h
 * @brief Fixed-size queue based on ring buffer
 */

#include "round_up_pow2_32.h" // round_up_pow2_32

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
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
 * @def FQUEUE_FOR_EACH(self, index, value)
 * @brief Iterate over the values in the queue from the front to back.
 *
 * @warning Modifying the queue under the iteration may result in errors.
 *
 * @param[in] self              Queue pointer.
 * @param[in] index             Temporary indexing variable. Should be `uint32_t`.
 * @param[out] value            Current value. Should be `VALUE_TYPE`.
 */
#ifndef FQUEUE_FOR_EACH
#define FQUEUE_FOR_EACH(self, index, value)                                                                          \
    for ((index) = 0; (index) < (self)->count                                                                        \
                      && ((value) = (self)->values[((self)->begin_index + (index)) & ((self)->capacity - 1)], true); \
         (index)++)
#endif

/**
 * @def FQUEUE_FOR_EACH_REVERSE(self, index, value)
 * @brief Iterate over the values in the queue from the back to front.
 *
 * @warning Modifying the queue under the iteration may result in errors.
 *
 * @param[in] self              Queue pointer.
 * @param[in] index             Temporary indexing variable. Should be `uint32_t`.
 * @param[out] value            Current value. Should be `VALUE_TYPE`.
 */
#ifndef FQUEUE_FOR_EACH_REVERSE
#define FQUEUE_FOR_EACH_REVERSE(self, index, value)                                                                    \
    for ((index) = 0; (index) < (self)->count                                                                          \
                      && ((value) = (self)->values[((self)->end_index - 1 - (index)) & ((self)->capacity - 1)], true); \
         (index)++)
#endif

/**
 * @def FQUEUE_CALC_SIZEOF(fqueue_name, capacity)
 *
 * @brief Calculate the size of the queue struct. No overflow checks.
 *
 * @param[in] fqueue_name       Defined queue NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      The equivalent size.
 */
#ifndef FQUEUE_CALC_SIZEOF
#define FQUEUE_CALC_SIZEOF(fqueue_name, capacity) \
    (uint32_t)(offsetof(struct fqueue_name, values) + capacity * sizeof(((struct fqueue_name *)0)->values[0]))
#endif

/**
 * @def FQUEUE_CALC_SIZEOF_OVERFLOWS(fqueue_name, capacity)
 *
 * @brief Check for a given capacity, if the equivalent size of the queue struct overflows.
 *
 * @param[in] fqueue_name       Defined queue NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      Whether the equivalent size overflows.
 */
#ifndef FQUEUE_CALC_SIZEOF_OVERFLOWS
#define FQUEUE_CALC_SIZEOF_OVERFLOWS(fqueue_name, capacity) \
    (capacity > (UINT32_MAX - offsetof(struct fqueue_name, values)) / sizeof(((struct fqueue_name *)0)->values[0]))
#endif

/**
 * @def NAME
 * @brief Prefix to queue type and operations. This must be manually defined
 *        before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#define NAME fqueue
#error "Must define NAME."
#else
#define FQUEUE_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Queue value type. This must be manually defined before including this
 *        header file.
 *
 * Is undefined after header is included.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#define FUNCTION_DEFINITIONS
#define TYPE_DEFINITIONS
#error "Must define VALUE_TYPE."
#endif

/**
 * @def FUNCTION_LINKAGE
 * @brief Specify function linkage e.g. static inline
 */
#ifndef FUNCTION_LINKAGE
#define FUNCTION_LINKAGE
#endif

/// @cond DO_NOT_DOCUMENT
#define FQUEUE_TYPE     struct FQUEUE_NAME
#define FQUEUE_INIT     JOIN(FQUEUE_NAME, init)
#define FQUEUE_IS_EMPTY JOIN(FQUEUE_NAME, is_empty)
#define FQUEUE_IS_FULL  JOIN(FQUEUE_NAME, is_full)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @def TYPE_DEFINITIONS
 * @brief Define the types
 */
#ifdef TYPE_DEFINITIONS

/**
 * @brief Generated queue struct type for a `VALUE_TYPE`.
 */
struct FQUEUE_NAME {
    uint32_t begin_index; ///< Index used to track the front of the queue.
    uint32_t end_index;   ///< Index used to track the back of the queue.
    uint32_t count;       ///< Number of values.
    uint32_t capacity;    ///< Maximum number of values allocated for.
    VALUE_TYPE values[];  ///< Array of values.
};

#endif

// }}}

// function declarations: {{{

/**
 * @brief Initialize a queue struct, given a (power-of-2) capacity.
 *
 * @param[in] self              Queue pointer
 * @param[in] pow2_capacity     Power of 2 capacity
 */
FUNCTION_LINKAGE FQUEUE_TYPE *JOIN(FQUEUE_NAME, init)(FQUEUE_TYPE *self, const uint32_t pow2_capacity);

/**
 * @brief Create an queue struct with a given capacity with malloc().
 *
 * @param[in] min_capacity      Maximum number of elements expected to be stored
 *
 * @return                      A pointer to the queue.
 * @retval NULL
 *   @li                        If malloc fails.
 *   @li                        If capacity is 0 or larger than UINT32_MAX / 2 + 1 or the equivalent size overflows.
 */
FUNCTION_LINKAGE FQUEUE_TYPE *JOIN(FQUEUE_NAME, create)(const uint32_t min_capacity);

/**
 * @brief Destroy an queue struct and free the underlying memory with free().
 *
 * @warning May not be called twice in a row on the same object.
 *
 * @param[in] self              The queue pointer.
 */
FUNCTION_LINKAGE void JOIN(FQUEUE_NAME, destroy)(FQUEUE_TYPE *self);

/**
 * @brief Return whether the queue is empty.
 *
 * @param[in] self              The queue pointer.
 *
 * @return                      Whether the queue is empty.
 */
FUNCTION_LINKAGE bool JOIN(FQUEUE_NAME, is_empty)(const FQUEUE_TYPE *self);

/**
 * @brief Return whether the queue is full.
 *
 * @param[in] self              The queue pointer.
 *
 * @return                      Whether the queue is full.
 */
FUNCTION_LINKAGE bool JOIN(FQUEUE_NAME, is_full)(const FQUEUE_TYPE *self);

/**
 * @brief Get the value at index.
 *
 * @note Index starts from the front as `0` and is counted upward to `count - 1`
 *       as back.
 *
 * @param[in] self              The queue pointer.
 * @param[in] index             The index to retrieve to value from.
 *
 * @return                      The value at `index`.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, at)(const FQUEUE_TYPE *self, const uint32_t index);

/**
 * @brief Get the value from the front of a non-empty queue.
 *
 * @param[in] self              The queue pointer.
 *
 * @return                      The front value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, get_front)(const FQUEUE_TYPE *self);

/**
 * @brief Get the value from the back of a non-empty queue.
 *
 * @param[in] self              The queue pointer.
 *
 * @return                      The back value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, get_back)(const FQUEUE_TYPE *self);

/**
 * @brief Peek a non-empty queue and get it's next to-be-dequeued value.
 *
 * @param[in] self              The queue pointer.
 *
 * @return                      The next to-be-dequeued value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, peek)(const FQUEUE_TYPE *self);

/**
 * @brief Enqueue a value at the back of a non-full queue.
 *
 * @param[in] self              The queue pointer.
 * @param[in] value             The value to enqueue.
 */
FUNCTION_LINKAGE bool JOIN(FQUEUE_NAME, enqueue)(FQUEUE_TYPE *self, const VALUE_TYPE value);

/**
 * @brief Dequeue a value from the front of a non-empty queue.
 *
 * @param[in] self              The queue pointer.
 *
 * @return                      The front value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, dequeue)(FQUEUE_TYPE *self);

/**
 * @brief Clear the elements in the queue.
 *
 * @param[in] self              The queue pointer.
 */
FUNCTION_LINKAGE void JOIN(FQUEUE_NAME, clear)(FQUEUE_TYPE *self);

/**
 * @brief Copy the values from a source queue to a destination queue.
 *
 * @param[in,out] dest_ptr      The destination queue.
 * @param[in] src_ptr           The source queue.
 */
FUNCTION_LINKAGE void JOIN(FQUEUE_NAME, copy)(FQUEUE_TYPE *restrict dest_ptr, const FQUEUE_TYPE *restrict src_ptr);

// }}}

// function definitions: {{{

#ifdef FUNCTION_DEFINITIONS

FUNCTION_LINKAGE FQUEUE_TYPE *JOIN(FQUEUE_NAME, init)(FQUEUE_TYPE *self, const uint32_t pow2_capacity)
{
    assert(self);
    assert(IS_POW2(pow2_capacity));

    self->begin_index = self->end_index = 0;
    self->count = 0;
    self->capacity = pow2_capacity;

    return self;
}

FUNCTION_LINKAGE FQUEUE_TYPE *JOIN(FQUEUE_NAME, create)(const uint32_t min_capacity)
{
    if (min_capacity == 0 || min_capacity > UINT32_MAX / 2 + 1) {
        return NULL;
    }

    const uint32_t capacity = round_up_pow2_32(min_capacity);

    if (FQUEUE_CALC_SIZEOF_OVERFLOWS(FQUEUE_NAME, capacity)) {
        return NULL;
    }

    const uint32_t size = FQUEUE_CALC_SIZEOF(FQUEUE_NAME, capacity);

    FQUEUE_TYPE *self = (FQUEUE_TYPE *)calloc(1, size);

    if (!self) {
        return NULL;
    }

    FQUEUE_INIT(self, capacity);

    return self;
}

FUNCTION_LINKAGE void JOIN(FQUEUE_NAME, destroy)(FQUEUE_TYPE *self)
{
    assert(self != NULL);

    free(self);
}

FUNCTION_LINKAGE bool JOIN(FQUEUE_NAME, is_empty)(const FQUEUE_TYPE *self)
{
    assert(self != NULL);

    return self->count == 0;
}

FUNCTION_LINKAGE bool JOIN(FQUEUE_NAME, is_full)(const FQUEUE_TYPE *self)
{
    assert(self != NULL);

    return self->count == self->capacity;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, at)(const FQUEUE_TYPE *self, const uint32_t index)
{
    assert(self != NULL);
    assert(index < self->count);

    const uint32_t index_mask = (self->capacity - 1);

    return self->values[(self->begin_index + index) & index_mask];
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, get_front)(const FQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(!FQUEUE_IS_EMPTY(self));

    return self->values[self->begin_index];
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, get_back)(const FQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(!FQUEUE_IS_EMPTY(self));

    const uint32_t index_mask = (self->capacity - 1);

    return self->values[(self->end_index - 1) & index_mask];
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, peek)(const FQUEUE_TYPE *self)
{
    return JOIN(FQUEUE_NAME, get_front)(self);
}

FUNCTION_LINKAGE bool JOIN(FQUEUE_NAME, enqueue)(FQUEUE_TYPE *self, const VALUE_TYPE value)
{
    assert(self != NULL);
    assert(!FQUEUE_IS_FULL(self));

    const uint32_t index_mask = (self->capacity - 1);

    self->values[self->end_index] = value;
    self->end_index++;
    self->end_index &= index_mask;
    self->count++;

    return true;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FQUEUE_NAME, dequeue)(FQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(!FQUEUE_IS_EMPTY(self));

    const uint32_t index_mask = (self->capacity - 1);

    const VALUE_TYPE value = self->values[self->begin_index];
    self->begin_index++;
    self->begin_index &= index_mask;
    self->count--;

    return value;
}

FUNCTION_LINKAGE void JOIN(FQUEUE_NAME, clear)(FQUEUE_TYPE *self)
{
    assert(self != NULL);

    self->count = 0;
    self->begin_index = self->end_index = 0;
}

FUNCTION_LINKAGE void JOIN(FQUEUE_NAME, copy)(FQUEUE_TYPE *restrict dest_ptr, const FQUEUE_TYPE *restrict src_ptr)
{
    assert(src_ptr != NULL);
    assert(dest_ptr != NULL);
    assert(src_ptr->count <= dest_ptr->capacity);
    assert(FQUEUE_IS_EMPTY(dest_ptr));

    const uint32_t src_begin_index = src_ptr->begin_index;
    const uint32_t src_index_mask = src_ptr->capacity - 1;

    for (uint32_t i = 0; i < src_ptr->count; i++) {
        dest_ptr->values[i] = src_ptr->values[(src_begin_index + i) & src_index_mask];
    }

    dest_ptr->count = src_ptr->count;
    dest_ptr->begin_index = 0;
    dest_ptr->end_index = src_ptr->count;
}

#endif

// }}}

// macro undefs: {{{

#undef NAME
#undef VALUE_TYPE
#undef FUNCTION_LINKAGE
#undef FUNCTION_DEFINITIONS
#undef TYPE_DEFINITIONS

#undef FQUEUE_NAME
#undef FQUEUE_TYPE
#undef FQUEUE_CALC_SIZEOF
#undef FQUEUE_INIT
#undef FQUEUE_IS_EMPTY
#undef FQUEUE_IS_FULL

// }}}

// vim: ft=c fdm=marker

/*  fstack_template.h
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
 * @file fstack_template.h
 * @brief Fixed-size array-based stack
 */

/**
 * @example fstack_example.c
 * Example of how `fstack.h` header file is used in practice.
 */

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
 * @def FSTACK_FOR_EACH(self, index, value)
 * @brief Iterate over the values in the stack from the top to bottom.
 *
 * @warning Modifying the stack under the iteration may result in errors.
 *
 * @param[in] self              Stack pointer.
 * @param[in] index             Temporary indexing variable. Should be `uint32_t`.
 * @param[out] value            Current value. Should be `VALUE_TYPE`.
 */
#ifndef FSTACK_FOR_EACH
#define FSTACK_FOR_EACH(self, index, value) \
    for ((index) = (self)->count; (index) > 0 && ((value) = (self)->values[(index) - 1], true); (index)--)
#endif

/**
 * @def FSTACK_FOR_EACH_REVERSE(self, index, value)
 * @brief Iterate over the values in the stack from the bottom to top.
 *
 * @warning Modifying the stack under the iteration may result in errors.
 *
 * @param[in] self              Stack pointer.
 * @param[in] index             Temporary indexing variable. Should be `uint32_t`.
 * @param[out] value            Current value. Should be `VALUE_TYPE`.
 */
#ifndef FSTACK_FOR_EACH_REVERSE
#define FSTACK_FOR_EACH_REVERSE(self, index, value) \
    for ((index) = 0; (index) < (self)->count && ((value) = (self)->values[(index)], true); (index)++)
#endif

/**
 * @def FSTACK_CALC_SIZEOF(fstack_name, capacity)
 *
 * @brief Calculate the size of the stack struct. No overflow checks.
 *
 * @param[in] fstack_name       Defined stack NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      The equivalent size.
 */
#ifndef FSTACK_CALC_SIZEOF
#define FSTACK_CALC_SIZEOF(fstack_name, capacity) \
    (uint32_t)(offsetof(struct fstack_name, values) + capacity * sizeof(((struct fstack_name *)0)->values[0]))
#endif

/**
 * @def FSTACK_CALC_SIZEOF_OVERFLOWS(fstack_name, capacity)
 *
 * @brief Check for a given capacity, if the equivalent size of the stack struct overflows.
 *
 * @param[in] fstack_name       Defined stack NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      Whether the equivalent size overflows.
 */
#ifndef FSTACK_CALC_SIZEOF_OVERFLOWS
#define FSTACK_CALC_SIZEOF_OVERFLOWS(fstack_name, capacity) \
    (capacity > (UINT32_MAX - offsetof(struct fstack_name, values)) / sizeof(((struct fstack_name *)0)->values[0]))
#endif

/**
 * @def NAME
 * @brief Prefix to stack type and operations. This must be manually defined
 *        before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#define NAME fstack
#error "Must define NAME."
#else
#define FSTACK_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Stack value type. This must be manually defined before including this
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
#define FSTACK_TYPE     struct FSTACK_NAME
#define FSTACK_IS_EMPTY JOIN(FSTACK_NAME, is_empty)
#define FSTACK_IS_FULL  JOIN(FSTACK_NAME, is_full)
#define FSTACK_INIT     JOIN(FSTACK_NAME, init)
/// @endcond

// }}}

// type definitions: {{{

/**
 * @def TYPE_DEFINITIONS
 * @brief Define the types
 */
#ifdef TYPE_DEFINITIONS

/**
 * @brief Generated stack struct type for a given `VALUE_TYPE`.
 */
struct FSTACK_NAME {
    uint32_t count;      ///< number of values.
    uint32_t capacity;   ///< maximum number of values allocated for.
    VALUE_TYPE values[]; ///< array of values.
};

#endif

// }}}

// function declarations: {{{

/**
 * @brief Initialize a stack struct, given a capacity.
 *
 * @param[in] self              Stack pointer
 * @param[in] capacity          Capacity
 */
FUNCTION_LINKAGE FSTACK_TYPE *JOIN(FSTACK_NAME, init)(FSTACK_TYPE *self, const uint32_t capacity);

/**
 * @brief Create an stack struct with a given capacity with malloc().
 *
 * @param[in] capacity          Maximum number of elements.
 *
 * @return                      A pointer to the stack.
 * @retval NULL
 *   @li                        If capacity is 0 or the equivalent size overflows
 *   @li                        If malloc fails.
 */
FUNCTION_LINKAGE FSTACK_TYPE *JOIN(FSTACK_NAME, create)(const uint32_t capacity);

/**
 * @brief Destroy an stack struct and free the underlying memory with free().
 *
 * @warning May not be called twice in a row on the same object.
 *
 * @param[in] self              The stack pointer.
 */
FUNCTION_LINKAGE void JOIN(FSTACK_NAME, destroy)(FSTACK_TYPE *self);

/**
 * @brief Return whether the stack is empty.
 *
 * @param[in] self              The stack pointer.
 *
 * @return                      Whether the stack is empty.
 */
FUNCTION_LINKAGE bool JOIN(FSTACK_NAME, is_empty)(const FSTACK_TYPE *self);

/**
 * @brief Return whether the stack is full.
 *
 * @param[in] self              The stack pointer.
 *
 * @return                      Whether the stack is full.
 */
FUNCTION_LINKAGE bool JOIN(FSTACK_NAME, is_full)(const FSTACK_TYPE *self);

/**
 * @brief Get the value at index.
 *
 * @note Index starts from the top as `0` and is counted upward to `count - 1`
 *       as bottom.
 *
 * @param[in] self              The stack pointer.
 * @param[in] index             The index to retrieve to value from.
 *
 * @return                      The value at `index`.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, at)(const FSTACK_TYPE *self, const uint32_t index);

/**
 * @brief Get the value from the top of a non-empty stack.
 *
 * @param[in] self              The stack pointer.
 *
 * @return                      The top value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, get_top)(const FSTACK_TYPE *self);

/**
 * @brief Get the value from the bottom of a non-empty stack.
 *
 * @param[in] self              The stack pointer.
 *
 * @return                      The bottom value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, get_bottom)(const FSTACK_TYPE *self);

/**
 * @brief Return whether the stack is empty.
 *
 * @param[in] self              The stack pointer.
 *
 * @return                      Whether the stack is empty.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, peek)(const FSTACK_TYPE *self);

/**
 * @brief Push a value onto a non-full stack.
 *
 * @param[in] self              The stack pointer.
 * @param[in] value             The value.
 */
FUNCTION_LINKAGE void JOIN(FSTACK_NAME, push)(FSTACK_TYPE *self, const VALUE_TYPE value);

/**
 * @brief Pop a value away from a non-empty stack.
 *
 * @param[in] self              The stack pointer.
 *
 * @return                      The top value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, pop)(FSTACK_TYPE *self);

/**
 * @brief Clear the elements in the stack.
 *
 * @param[in] self              The stack pointer.
 */
FUNCTION_LINKAGE void JOIN(FSTACK_NAME, clear)(FSTACK_TYPE *self);

/**
 * @brief Copy the values from a source stack to a destination stack.
 *
 * @param[in,out] dest_ptr      The destination stack.
 * @param[in] src_ptr           The source stack.
 */
FUNCTION_LINKAGE void JOIN(FSTACK_NAME, copy)(FSTACK_TYPE *restrict dest_ptr, const FSTACK_TYPE *restrict src_ptr);

// }}}

// function definitions: {{{

/**
 * @def FUNCTION_DEFINITIONS
 * @brief Define the functions
 */
#ifdef FUNCTION_DEFINITIONS

FUNCTION_LINKAGE FSTACK_TYPE *JOIN(FSTACK_NAME, init)(FSTACK_TYPE *self, const uint32_t capacity)
{
    assert(self);

    self->count = 0;
    self->capacity = capacity;

    return self;
}

FUNCTION_LINKAGE FSTACK_TYPE *JOIN(FSTACK_NAME, create)(const uint32_t capacity)
{
    if (capacity == 0 || FSTACK_CALC_SIZEOF_OVERFLOWS(FSTACK_NAME, capacity)) {
        return NULL;
    }

    const uint32_t size = FSTACK_CALC_SIZEOF(FSTACK_NAME, capacity);

    FSTACK_TYPE *self = (FSTACK_TYPE *)calloc(1, size);

    if (!self) {
        return NULL;
    }

    FSTACK_INIT(self, capacity);

    return self;
}

FUNCTION_LINKAGE void JOIN(FSTACK_NAME, destroy)(FSTACK_TYPE *self)
{
    assert(self != NULL);

    free(self);
}

FUNCTION_LINKAGE bool JOIN(FSTACK_NAME, is_empty)(const FSTACK_TYPE *self)
{
    assert(self != NULL);

    return self->count == 0;
}

FUNCTION_LINKAGE bool JOIN(FSTACK_NAME, is_full)(const FSTACK_TYPE *self)
{
    assert(self != NULL);

    return self->count == self->capacity;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, at)(const FSTACK_TYPE *self, const uint32_t index)
{
    assert(self != NULL);
    assert(index < self->count);

    return self->values[self->count - 1 - index];
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, get_top)(const FSTACK_TYPE *self)
{
    assert(self != NULL);
    assert(FSTACK_IS_EMPTY(self) == false);

    return self->values[self->count - 1];
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, get_bottom)(const FSTACK_TYPE *self)
{
    assert(self != NULL);
    assert(FSTACK_IS_EMPTY(self) == false);

    return self->values[0];
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, peek)(const FSTACK_TYPE *self)
{
    return JOIN(FSTACK_NAME, get_top)(self);
}

FUNCTION_LINKAGE void JOIN(FSTACK_NAME, push)(FSTACK_TYPE *self, const VALUE_TYPE value)
{
    assert(self != NULL);
    assert(FSTACK_IS_FULL(self) == false);

    self->values[self->count++] = value;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FSTACK_NAME, pop)(FSTACK_TYPE *self)
{
    assert(self != NULL);
    assert(FSTACK_IS_EMPTY(self) == false);

    return self->values[--self->count];
}

FUNCTION_LINKAGE void JOIN(FSTACK_NAME, clear)(FSTACK_TYPE *self)
{
    assert(self != NULL);
    self->count = 0;
}

FUNCTION_LINKAGE void JOIN(FSTACK_NAME, copy)(FSTACK_TYPE *restrict dest_ptr, const FSTACK_TYPE *restrict src_ptr)
{
    assert(src_ptr != NULL);
    assert(dest_ptr != NULL);
    assert(src_ptr->count <= dest_ptr->capacity);
    assert(FSTACK_IS_EMPTY(dest_ptr));

    for (uint32_t i = 0; i < src_ptr->count; i++) {
        dest_ptr->values[i] = src_ptr->values[i];
    }
    dest_ptr->count = src_ptr->count;
}

#endif

// }}}

// macro undefs: {{{
#undef NAME
#undef VALUE_TYPE
#undef FUNCTION_LINKAGE
#undef FUNCTION_DEFINITIONS
#undef TYPE_DEFINITIONS

#undef FSTACK_NAME
#undef FSTACK_TYPE
#undef FSTACK_IS_EMPTY
#undef FSTACK_IS_FULL
#undef FSTACK_INIT

// }}}

// vim: ft=c fdm=marker

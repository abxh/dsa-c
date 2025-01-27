/*  fpqueue_template.h
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
 * @file fpqueue_template.h
 * @brief Fixed-size priority queue based on binary (max-)heap.
 *
 * The following macros must be defined:
 *  @li `NAME`
 *  @li `VALUE_TYPE`
 *
 * Source used:
 * @li CLRS
 */

/**
 * @example fpqueue_example.c
 * Example of how `fpqueue.h` header file is used in practice.
 */

#ifdef __cplusplus
#ifdef __GNUC__
#define restrict __restrict__
#else
#define restrict
#endif
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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
 * @def FPQUEUE_LEFT_CHILD(index)
 * @brief Given an element index, get the index of the left child.
 *
 * @param[in] index The element index.
 */
#ifndef FPQUEUE_LEFT_CHILD
#define FPQUEUE_LEFT_CHILD(index) (2 * (index) + 1)
#endif

/**
 * @def FPQUEUE_RIGHT_CHILD(index)
 * @brief Given an element index, get the index of the right child.
 *
 * @param[in] index The element index.
 */
#ifndef FPQUEUE_RIGHT_CHILD
#define FPQUEUE_RIGHT_CHILD(index) (2 * (index) + 2)
#endif

/**
 * @def FPQUEUE_PARENT(index)
 * @brief Given an element index, get the index of the parent.
 *
 * @param[in] index The element index.
 */
#ifndef FPQUEUE_PARENT
#define FPQUEUE_PARENT(index) (((index) - 1) / 2)
#endif

/**
 * @def FPQUEUE_FOR_EACH(self, index, value_)
 * @brief Iterate over the values in the priority queue in breadth-first order.
 *
 * @warning Modifying the priority queue under the iteration may result in
 *          errors.
 *
 * @param[in] self              Priority queue pointer.
 * @param[in] index             Temporary indexing variable. Should be `uint32_t`.
 * @param[out] value_           Current value. Should be `VALUE_TYPE`.
 */
#ifndef FPQUEUE_FOR_EACH
#define FPQUEUE_FOR_EACH(self, index, value_) \
    for ((index) = 0; (index) < (self)->count && ((value_) = (self)->elements[(index)].value, true); (index)++)
#endif

/**
 * @def FPQUEUE_CALC_SIZEOF(fpqueue_name, capacity)
 *
 * @brief Calculate the size of the pqueue struct. No overflow checks.
 *
 * @param[in] fpqueue_name      Defined pqueue NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      The equivalent size.
 */
#ifndef FPQUEUE_CALC_SIZEOF
#define FPQUEUE_CALC_SIZEOF(fpqueue_name, capacity) \
    (uint32_t)(offsetof(struct fpqueue_name, elements) + capacity * sizeof(((struct fpqueue_name *)0)->elements[0]))
#endif

/**
 * @def FPQUEUE_CALC_SIZEOF_OVERFLOWS(fpqueue_name, capacity)
 *
 * @brief Check for a given capacity, if the equivalent size of the pqueue struct overflows.
 *
 * @param[in] fpqueue_name      Defined pqueue NAME.
 * @param[in] capacity          Capacity input.
 *
 * @return                      Whether the equivalent size overflows.
 */
#ifndef FPQUEUE_CALC_SIZEOF_OVERFLOWS
#define FPQUEUE_CALC_SIZEOF_OVERFLOWS(fpqueue_name, capacity) \
    (capacity                                                 \
     > (UINT32_MAX - offsetof(struct fpqueue_name, elements)) / sizeof(((struct fpqueue_name *)0)->elements[0]))
#endif

/**
 * @def NAME
 * @brief Prefix to priority queue types and operations. This must be manually
 *        defined before including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef NAME
#error "Must define NAME."
#else
#define FPQUEUE_NAME NAME
#endif

/**
 * @def VALUE_TYPE
 * @brief Priority queue value type. This must be manually defined before
 *        including this header file.
 *
 * Is undefined after header is included.
 */
#ifndef VALUE_TYPE
#define VALUE_TYPE int
#define FUNCTION_DEFINITIONS
#define TYPE_DEFINITIONS
#error "Must declare VALUE_TYPE."
#endif

/**
 * @def FUNCTION_LINKAGE
 * @brief Specify function linkage e.g. static inline
 */
#ifndef FUNCTION_LINKAGE
#define FUNCTION_LINKAGE
#endif

/// @cond DO_NOT_DOCUMENT
#define FPQUEUE_TYPE         struct FPQUEUE_NAME
#define FPQUEUE_ELEMENT_TYPE struct JOIN(FPQUEUE_NAME, element)
#define FPQUEUE_INIT         JOIN(FPQUEUE_NAME, init)
#define FPQUEUE_IS_EMPTY     JOIN(FPQUEUE_NAME, is_empty)
#define FPQUEUE_IS_FULL      JOIN(FPQUEUE_NAME, is_full)
#define FPQUEUE_UPHEAP       JOIN(internal, JOIN(FPQUEUE_NAME, upheap))
#define FPQUEUE_DOWNHEAP     JOIN(internal, JOIN(FPQUEUE_NAME, downheap))
/// @endcond

// }}}

// type definitions: {{{

/**
 * @def TYPE_DEFINITIONS
 * @brief Define the types
 */
#ifdef TYPE_DEFINITIONS

/**
 * @brief Generated priority queue element struct type for a given `VALUE_TYPE`.
 */
struct JOIN(FPQUEUE_NAME, element) {
    uint32_t priority; ///< Element priority (highest is next to-be-popped).
    VALUE_TYPE value;  ///< Element value member.
};

/**
 * @brief Generated priority queue struct type for a given `VALUE_TYPE`.
 */
struct FPQUEUE_NAME {
    uint32_t count;                  ///< Number of non-empty elements.
    uint32_t capacity;               ///< Number of elements allocated for.
    FPQUEUE_ELEMENT_TYPE elements[]; ///< Array of elements.
};

#endif

// }}}

// function declarations: {{{

/**
 * @brief Initialize a priority queue struct, given a capacity.
 *
 * @param[in] self              Priority queue pointer
 * @param[in] capacity          Capacity
 */
FUNCTION_LINKAGE FPQUEUE_TYPE *JOIN(FPQUEUE_NAME, init)(FPQUEUE_TYPE *self, const uint32_t capacity);

/**
 * @brief Create an priority queue struct with a given capacity with malloc().
 *
 * @param[in] capacity          Maximum number of elements expected to be stored.
 *
 * @return                      A pointer to the priority queue.
 * @retval NULL
 *   @li                        If capacity is 0 or the equivalent size overflows.
 *   @li                        If malloc fails.
 */
FUNCTION_LINKAGE FPQUEUE_TYPE *JOIN(FPQUEUE_NAME, create)(const uint32_t capacity);

/**
 * @brief Destroy an priority queue struct and free the underlying memory with free().
 *
 * @warning May not be called twice in a row on the same object.
 *
 * @param[in] self              The priority queue pointer.
 */
FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, destroy)(FPQUEUE_TYPE *self);

/**
 * @brief Return whether the priority queue is empty.
 *
 * @param[in] self              The priority queue pointer.
 *
 * @return                      Whether the priority queue is empty.
 */
FUNCTION_LINKAGE bool JOIN(FPQUEUE_NAME, is_empty)(const FPQUEUE_TYPE *self);

/**
 * @brief Return whether the priority queue is full.
 *
 * @param[in] self              The priority queue pointer.
 *
 * @return                      Whether the priority queue is full.
 */
FUNCTION_LINKAGE bool JOIN(FPQUEUE_NAME, is_full)(const FPQUEUE_TYPE *self);

/**
 * @brief Get the max priority value in a non-empty priority queue.
 *
 * @param[in] self              The priority queue pointer.
 *
 * @return                      The max priority value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FPQUEUE_NAME, get_max)(const FPQUEUE_TYPE *self);

/**
 * @brief Peek a non-empty priority queue and get it's next to-be-popped (max priority)
 *        value.
 *
 * @param[in] self              The priority queue pointer.
 *
 * @return                      The next to-be-popped (max priority) value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FPQUEUE_NAME, peek)(const FPQUEUE_TYPE *self);

/**
 * @brief Pop the max priority value away from a non-empty priority queue.
 *
 * @param[in] self              The priority queue pointer.
 *
 * @return                      The max value.
 */
FUNCTION_LINKAGE VALUE_TYPE JOIN(FPQUEUE_NAME, pop_max)(FPQUEUE_TYPE *self);

/**
 * @brief Push a value with given priority onto a non-full priority queue.
 *
 * @param[in] self              The priority queue pointer.
 * @param[in] value             The value.
 * @param[in] priority          The priority (with large number meaning high
 *                              priority and vice versa).
 */
FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, push)(FPQUEUE_TYPE *self, VALUE_TYPE value, const uint32_t priority);

/**
 * @brief Push a value with given priority onto a non-full priority queue.
 *
 * @param[in] self              The priority queue pointer.
 * @param[in] value             The value.
 * @param[in] priority          The priority (with large number meaning high
 *                              priority and vice versa).
 */
FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, push)(FPQUEUE_TYPE *self, VALUE_TYPE value, const uint32_t priority);

/**
 * @brief Clear the elements in the priority queue.
 *
 * @param[in] self              The priority queue pointer.
 */
FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, clear)(FPQUEUE_TYPE *self);

/**
 * @brief Copy the values from a source priority queue to a destination priority
 *        queue.
 *
 * @param[in,out] dest_ptr      The destination priority queue.
 * @param[in] src_ptr           The source priority queue.
 */
FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, copy)(FPQUEUE_TYPE *restrict dest_ptr, const FPQUEUE_TYPE *restrict src_ptr);

// @}}}

// function definitions: {{{

/**
 * @def FUNCTION_DEFINITIONS
 * @brief Define the functions
 */
#ifdef FUNCTION_DEFINITIONS

/// @cond DO_NOT_DOCUMENT

/* push a node down the heap. for restoring the heap property after insertion */
static inline void JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(FPQUEUE_TYPE *self, const uint32_t index);

/* push a node up the heap. for restoring the heap property after deletion */
static inline void JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(FPQUEUE_TYPE *self, uint32_t index);

/// @endcond

FUNCTION_LINKAGE FPQUEUE_TYPE *JOIN(FPQUEUE_NAME, init)(FPQUEUE_TYPE *self, const uint32_t capacity)
{
    assert(self);

    self->count = 0;
    self->capacity = capacity;

    return self;
}

FUNCTION_LINKAGE FPQUEUE_TYPE *JOIN(FPQUEUE_NAME, create)(const uint32_t capacity)
{
    if (capacity == 0 || FPQUEUE_CALC_SIZEOF_OVERFLOWS(FPQUEUE_NAME, capacity)) {
        return NULL;
    }

    const uint32_t size = FPQUEUE_CALC_SIZEOF(FPQUEUE_NAME, capacity);

    FPQUEUE_TYPE *self = (FPQUEUE_TYPE *)calloc(1, size);

    if (!self) {
        return NULL;
    }

    FPQUEUE_INIT(self, capacity);

    return self;
}

FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, destroy)(FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    free(self);
}

FUNCTION_LINKAGE bool JOIN(FPQUEUE_NAME, is_empty)(const FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    return self->count == 0;
}

FUNCTION_LINKAGE bool JOIN(FPQUEUE_NAME, is_full)(const FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    return self->count == self->capacity;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FPQUEUE_NAME, get_max)(const FPQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(FPQUEUE_IS_EMPTY(self) == false);

    return self->elements[0].value;
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FPQUEUE_NAME, peek)(const FPQUEUE_TYPE *self)
{
    return JOIN(FPQUEUE_NAME, get_max)(self);
}

FUNCTION_LINKAGE VALUE_TYPE JOIN(FPQUEUE_NAME, pop_max)(FPQUEUE_TYPE *self)
{
    assert(self != NULL);
    assert(FPQUEUE_IS_EMPTY(self) == false);

    VALUE_TYPE max_priority_value = self->elements[0].value;

    self->elements[0] = self->elements[self->count - 1];

    self->count--;

    JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(self, 0);

    return max_priority_value;
}

FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, push)(FPQUEUE_TYPE *self, VALUE_TYPE value, const uint32_t priority)
{
    assert(self != NULL);
    assert(FPQUEUE_IS_FULL(self) == false);

    const uint32_t index = self->count;

    self->elements[index] = (FPQUEUE_ELEMENT_TYPE){.priority = priority, .value = value};

    self->count++;

    JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(self, index);
}

FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, clear)(FPQUEUE_TYPE *self)
{
    assert(self != NULL);

    self->count = 0;
}

FUNCTION_LINKAGE void JOIN(FPQUEUE_NAME, copy)(FPQUEUE_TYPE *restrict dest_ptr, const FPQUEUE_TYPE *restrict src_ptr)
{
    assert(src_ptr != NULL);
    assert(dest_ptr != NULL);
    assert(src_ptr->count <= dest_ptr->capacity);
    assert(FPQUEUE_IS_EMPTY(dest_ptr));

    for (uint32_t i = 0; i < src_ptr->count; i++) {
        dest_ptr->elements[i] = src_ptr->elements[i];
    }
    dest_ptr->count = src_ptr->count;
}

/// @cond DO_NOT_DOCUMENT

static inline void JOIN(internal, JOIN(FPQUEUE_NAME, upheap))(FPQUEUE_TYPE *self, uint32_t index)
{
    assert(self != NULL);
    assert(index < self->count);

    uint32_t parent;
    while (index > 0) {
        parent = FPQUEUE_PARENT(index);

        const bool sorted = self->elements[parent].priority >= self->elements[index].priority;

        if (sorted) {
            break;
        }

        const FPQUEUE_ELEMENT_TYPE temp = self->elements[index];
        self->elements[index] = self->elements[parent];
        self->elements[parent] = temp;

        index = parent;
    }
}

static inline void JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(FPQUEUE_TYPE *self, const uint32_t index)
{
    assert(self != NULL);
    assert(self->count == 0 || index < self->count);

    const uint32_t l = FPQUEUE_LEFT_CHILD(index);
    const uint32_t r = FPQUEUE_RIGHT_CHILD(index);

    uint32_t largest = index;
    if (l < self->count && self->elements[l].priority > self->elements[index].priority) {
        largest = l;
    }
    if (r < self->count && self->elements[r].priority > self->elements[largest].priority) {
        largest = r;
    }

    if (largest == index) {
        return;
    }

    const FPQUEUE_ELEMENT_TYPE temp = self->elements[index];
    self->elements[index] = self->elements[largest];
    self->elements[largest] = temp;

    JOIN(internal, JOIN(FPQUEUE_NAME, downheap))(self, largest);
}
/// @endcond

#endif

// }}}

// macro undefs: {{{

#undef NAME
#undef VALUE_TYPE
#undef FUNCTION_LINKAGE
#undef FUNCTION_DEFINITIONS
#undef TYPE_DEFINITIONS

#undef FPQUEUE_NAME
#undef FPQUEUE_TYPE
#undef FPQUEUE_ELEMENT_TYPE
#undef FPQUEUE_INIT
#undef FPQUEUE_IS_EMPTY
#undef FPQUEUE_IS_FULL
#undef FPQUEUE_CALC_SIZEOF
#undef FHASHTABLE_UPHEAP
#undef FHASHTABLE_DOWNHEAP

// }}}

#ifdef __cplusplus
}
#endif

// vim: ft=c fdm=marker

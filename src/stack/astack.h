/*
    array-based stack

    The following macros are defined:
    - astack_for_each

    The following structs are defined:
    - astack_T

    The following functions are generated for a given value type t with label T:
    - astack_T_init
    - astack_T_deinit
    - astack_T_count
    - astack_T_is_empty
    - astack_T_is_full
    - astack_T_peek
    - astack_T_push
    - astack_T_pop
    - astack_T_resize

    A distinction between the value and label is made as the label
    cannot include spaces.
*/

#ifndef __ASTACK__H
#define __ASTACK__H

#include <assert.h> // assert
#include <stdlib.h> // size_t, NULL, malloc, calloc, free, reallocarray

#define astack_for_each(astack_p, value, index) \
    for ((index) = 0; ((index) < (astack_p)->count && ((value) = (astack_p)->arr_p[(index)], true)); (index)++)

#endif

#ifndef VALUE_TYPE
#error "Must declare VALUE_TYPE. Defaulting to int."
#define VALUE_TYPE int
#endif

#ifndef VALUE_LABEL
#error "Must declare VALUE_LABEL. Defaulting to 'int'."
#define VALUE_LABEL int
#endif

#define CAT(a, b) a##b
#define PASTE(a, b) CAT(a, b)
#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define astack_T JOIN(astack, VALUE_LABEL)

typedef struct {
    size_t count;
    size_t capacity;
    VALUE_TYPE* arr_p;
} astack_T;

static inline bool JOIN(astack_T, init)(astack_T** astack_pp, size_t capacity) {
    assert(astack_pp != NULL);
    assert(capacity != 0);
    *astack_pp = (astack_T*)malloc(sizeof(astack_T));
    if ((*astack_pp) == NULL) {
        return false;
    }
    (*astack_pp)->arr_p = (VALUE_TYPE*)calloc(capacity, sizeof(VALUE_TYPE));
    if ((*astack_pp)->arr_p == NULL) {
        free(*astack_pp);
        *astack_pp = NULL;
        return false;
    }
    (*astack_pp)->count = 0;
    (*astack_pp)->capacity = capacity;
    return true;
}

static inline bool JOIN(astack_T, deinit)(astack_T** astack_pp) {
    if (*astack_pp == NULL) {
        return false;
    }
    free((*astack_pp)->arr_p);
    free(*astack_pp);
    *astack_pp = NULL;
    return true;
}

static inline bool JOIN(astack_T, count)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count;
}

static inline bool JOIN(astack_T, is_empty)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count == 0;
}

static inline bool JOIN(astack_T, is_full)(const astack_T* astack_p) {
    assert(astack_p != NULL);
    return astack_p->count == astack_p->capacity;
}

static inline const VALUE_TYPE JOIN(astack_T, peek)(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(JOIN(astack_T, is_empty)(astack_p) == false);
    return astack_p->arr_p[astack_p->count - 1];
}

static inline void JOIN(astack_T, push)(astack_T* astack_p, const VALUE_TYPE value) {
    assert(astack_p != NULL);
    assert(JOIN(astack_T, is_full)(astack_p) == false);
    astack_p->arr_p[astack_p->count++] = value;
}

static inline VALUE_TYPE JOIN(astack_T, pop)(astack_T* astack_p) {
    assert(astack_p != NULL);
    assert(JOIN(astack_T, is_empty)(astack_p) == false);
    return ((VALUE_TYPE*)astack_p->arr_p)[--astack_p->count];
}

static inline bool JOIN(astack_T, resize)(astack_T* stack_p, size_t new_capacity) {
    assert(new_capacity != 0);
    VALUE_TYPE* new_arr_p = (VALUE_TYPE*)reallocarray(stack_p->arr_p, new_capacity, sizeof(VALUE_TYPE));
    if (new_arr_p == NULL) {
        return false;
    }
    stack_p->arr_p = new_arr_p;
    stack_p->capacity = new_capacity;
    return true;
}

#undef astack_T

#undef CAT
#undef PASTE
#undef JOIN

#undef VALUE_LABEL
#undef VALUE_TYPE

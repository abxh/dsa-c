#include <assert.h>  // assert
#include <stdbool.h> // bool
#include <stdlib.h>  // malloc, realloc, size_t, NULL
#include <string.h>  // memcpy
#include <stdint.h>  // SIZE_MAX

#include "stack.h"

Stack* stack_new(size_t capacity, size_t data_size) {
    assert(capacity != 0);
    assert(data_size != 0);
    if (capacity > SIZE_MAX / data_size) {
        return NULL;
    }
    Stack* stack_p = malloc(sizeof(Stack));
    if (stack_p == NULL) {
        return NULL;
    }
    stack_p->arr_p = malloc(capacity * data_size);
    if (stack_p->arr_p == NULL) {
        free(stack_p);
        return NULL;
    }
    stack_p->used = 0;
    stack_p->capacity = capacity;
    stack_p->data_size = data_size;
    return stack_p;
}

size_t stack_num_used(const Stack* stack_p) {
    return stack_p->used;
}

bool stack_isempty(const Stack* stack_p) {
    return stack_p->used == 0;
}

bool stack_isfull(const Stack* stack_p) {
    return stack_p->used == stack_p->capacity;
}

unsigned char* stack_peek(const Stack* stack_p) {
    assert(stack_p->used != 0);
    return stack_p->arr_p + stack_p->data_size * (stack_p->used - 1);
}

void stack_push(Stack* stack_p, const unsigned char* bytes) {
    assert(stack_p->used != stack_p->capacity);
    memcpy(stack_p->arr_p + stack_p->data_size * stack_p->used++, bytes, stack_p->data_size);
}

unsigned char* stack_pop(Stack* stack_p) {
    assert(stack_p->used != 0);
    return stack_p->arr_p + stack_p->data_size * --stack_p->used;
}

bool stack_resize(Stack* stack_p, size_t new_capacity) {
    assert(new_capacity != 0);
    if (new_capacity < stack_p->used) {
        return false;
    }
    else if (new_capacity == stack_p->capacity) {
        return true;
    }
    void* new_arr_p = realloc(stack_p->arr_p, new_capacity * stack_p->data_size);
    if (new_arr_p == NULL) {
        return false;
    }
    stack_p->arr_p = new_arr_p;
    stack_p->capacity = new_capacity;
    return true;
}

void stack_free(Stack** stack_pp) {
    if (*stack_pp == NULL) {
        return;
    }
    free((*stack_pp)->arr_p);
    free(*stack_pp);
    *stack_pp = NULL;
}

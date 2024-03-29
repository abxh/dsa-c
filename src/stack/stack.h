#pragma once

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

typedef struct StackNode {
    struct StackNode* next_p;
    void* value_p;
} StackNode;

typedef struct {
    StackNode* head_p;
    size_t value_size;
    size_t count;

    StackNode* freelist_p;
} Stack;

bool stack_init(Stack** stack_pp, const size_t value_size);

bool stack_deinit(Stack** stack_pp);

const StackNode* stack_peek_node(const Stack* stack_p);

size_t stack_count(const Stack* stack_p);

bool stack_is_empty(const Stack* stack_p);

void stack_push_node(Stack* stack_p, StackNode* node_p);

StackNode* stack_pop_node(Stack* stack_p);

StackNode* stacknode_create(Stack* stack_p);

void stacknode_free(Stack* stack_p, StackNode* node_p);

#define stack_for_each(stack_p, node_p, value)                                                                    \
    for ((node_p) = (stack_p)->head_p; (node_p) != NULL && ((value) = *(typeof(value)*)(node_p)->value_p, true); \
         (node_p) = (node_p)->next_p)

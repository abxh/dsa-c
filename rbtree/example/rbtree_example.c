
#include <stdio.h>

#include "rbtree.h"
#include "container_of.h"

typedef struct {
    struct rbtree_node node; // note: should be first member, if following C standard strictly.
    // attached values:
    char c;
} con_type;

#define arr_count(arr) sizeof(arr) / sizeof(*arr)

static void inorder_traverse_and_putchar(struct rbtree_node *n)
{
    if (n == NULL) {
        return;
    }
    const char c = container_of(n, con_type, node)->c;

    inorder_traverse_and_putchar(n->left_ptr);
    putchar(c);
    inorder_traverse_and_putchar(n->right_ptr);
}

static void deallocate(void *ptr)
{
    // do nothing. exists for demonstration purposes
    (void)(ptr);
}

void hello_world(void)
{
    struct {
        int key;
        char c;
    } pairs[] = {
        {-1, '$'}, {4, 'o'},  {11, '!'}, {2, 'l'}, {0, 'h'}, {6, 'w'}, {12, '\n'}, {5, ' '},
        {7, 'o'},  {10, 'd'}, {9, 'l'},  {1, 'e'}, {3, 'l'}, {8, 'r'}, {13, '\0'},
    };

    struct rbtree_node *rb;
    rbtree_init(&rb);

    assert(rbtree_is_empty(&rb));

    con_type cons[arr_count(pairs)];

    for (size_t i = 0; i < arr_count(pairs); i++) {
        cons[i].c = pairs[i].c;
        rbtree_node_init(&cons[i].node, pairs[i].key);

        if (!rbtree_contains_key(&rb, pairs[i].key)) {
            rbtree_insert_node(&rb, &cons[i].node);
        }
        else {
            assert(false);
        }
    }

    struct rbtree_node *node_ptr = rbtree_search_node(&rb, -1);
    if (node_ptr) {
        deallocate(rbtree_delete_node(&rb, node_ptr));
    }
    else {
        assert(false);
    }

    /* inorder_traverse_and_putchar(rb); */
    (void)(inorder_traverse_and_putchar);
}

int main(void)
{
    hello_world();
}

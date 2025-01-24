/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 15
    - N := 8192

    Non-mutating operation types / properties:
    - node_parent_ptr
    - node_is_black
    - node_is_red
    - is_empty
    - contains_key
    - get_value / get_value_mut
    - search_node
    - search_best_node

    Mutating operation types:
    - init
    - node_init
    - insert_node
    - delete_node

    Sources used for testing:
    http://www.maxgcoding.com/validating-red-black-trees/
*/

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "container_of.h"

#define NAME                       bst
#define KEY_TYPE                   int
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "rbtree_template.h"

#define NAME                       bstd
#define KEY_TYPE                   int
#define KEY_IS_STRICTLY_LESS(a, b) ((a) < (b))
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#define ALLOW_DUPLICATES
#include "rbtree_template.h"

struct extended_bst_node {
    struct bst_node node;
    unsigned int value;
};

static int max_int(const int a, const int b)
{
    return (a >= b) ? a : b;
}

static int count_height(const struct bst_node *root_ptr)
{
    if (root_ptr == NULL) {
        return 0;
    }
    int height = 0;
    if (root_ptr->left_ptr) {
        height = 1 + count_height(root_ptr->left_ptr);
    }
    if (root_ptr->right_ptr) {
        height = max_int(height, 1 + count_height(root_ptr->right_ptr));
    }
    return height;
}

static bool is_valid_binary_search_tree(const float left_key, const struct bst_node *root_ptr, const float right_key)
{
    if (root_ptr == NULL) {
        return true;
    }
    if (!(left_key < root_ptr->key && root_ptr->key < right_key)) { // note this depends on particulars
        return false;
    }
    return is_valid_binary_search_tree(left_key, root_ptr->left_ptr, (float)root_ptr->key)
        && is_valid_binary_search_tree((float)root_ptr->key, root_ptr->right_ptr, right_key);
}

static bool is_valid_binary_search_tree_w_duplicates(const float left_key, const struct bst_node *root_ptr,
                                                     const float right_key)
{
    if (root_ptr == NULL) {
        return true;
    }
    if (!(left_key <= root_ptr->key && root_ptr->key <= right_key)) { // note this depends on particulars
        return false;
    }
    return is_valid_binary_search_tree_w_duplicates(left_key, root_ptr->left_ptr, (float)root_ptr->key)
        && is_valid_binary_search_tree_w_duplicates((float)root_ptr->key, root_ptr->right_ptr, right_key);
}

static bool rbtree_check_for_no_consecutive_reds(const struct bst_node *parent_ptr, const struct bst_node *root_ptr)
{
    if (root_ptr == NULL) {
        return true;
    }
    if (parent_ptr && bst_node_is_red(parent_ptr) && bst_node_is_red(root_ptr)) {
        return false;
    }
    return rbtree_check_for_no_consecutive_reds(root_ptr, root_ptr->left_ptr)
        && rbtree_check_for_no_consecutive_reds(root_ptr, root_ptr->right_ptr);
}

static int rbtree_get_black_height_of_some_path(const struct bst_node *root_ptr)
{
    if (root_ptr == NULL) {
        return 1;
    }
    return (bst_node_is_black(root_ptr) ? 1 : 0) + rbtree_get_black_height_of_some_path(root_ptr->left_ptr);
}

static bool rbtree_check_equal_black_height(const struct bst_node *root_ptr, const int height)
{
    if (root_ptr == NULL) {
        return height == 1;
    }
    const int is_black = bst_node_is_black(root_ptr) ? 1 : 0;

    return rbtree_check_equal_black_height(root_ptr->left_ptr, height - is_black)
        && rbtree_check_equal_black_height(root_ptr->right_ptr, height - is_black);
}

static bool is_valid_red_black_tree(const struct bst_node *root_ptr)
{
    const bool is_valid_bst = is_valid_binary_search_tree(-INFINITY, root_ptr, INFINITY);
    const bool is_valid_234 = rbtree_check_for_no_consecutive_reds(NULL, root_ptr);
    const bool balanced_black_height =
        rbtree_check_equal_black_height(root_ptr, rbtree_get_black_height_of_some_path(root_ptr));

    return is_valid_bst && is_valid_234 && balanced_black_height;
}

static bool is_valid_red_black_tree_w_duplicates(const struct bst_node *root_ptr)
{
    const bool is_valid_bst_w_duplicates = is_valid_binary_search_tree_w_duplicates(-INFINITY, root_ptr, INFINITY);
    const bool is_valid_234 = rbtree_check_for_no_consecutive_reds(NULL, root_ptr);
    const bool balanced_black_height =
        rbtree_check_equal_black_height(root_ptr, rbtree_get_black_height_of_some_path(root_ptr));

    return is_valid_bst_w_duplicates && is_valid_234 && balanced_black_height;
}

static void preorder_traverse_and_print(const struct bst_node *root_ptr, bool print_children)
{
    if (root_ptr == NULL) {
        if (print_children) {
            printf(" ()");
        }
        return;
    }
    if (bst_node_is_red(root_ptr)) {
        printf(" ( [%d]", root_ptr->key);
    }
    else {
        printf(" ( %d", root_ptr->key);
    }
    bool dont_print_children = root_ptr->left_ptr == NULL && root_ptr->right_ptr == NULL;
    preorder_traverse_and_print(root_ptr->left_ptr, !dont_print_children);
    preorder_traverse_and_print(root_ptr->right_ptr, !dont_print_children);
    printf(" )");
}

int main(void)
{
    (void)(preorder_traverse_and_print);

    // N = 0
    {
        struct bst_node *bst;
        bst_init(&bst);
        assert(is_valid_red_black_tree(bst));
    }
    // N = 1
    {
        struct bst_node *bst;
        bst_init(&bst);

        assert(!bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));

        assert(bst == NULL);
        assert(bst_is_empty(&bst));

        assert(is_valid_red_black_tree(bst));
    }
    // N = 1, insert_node
    {
        struct bst_node *bst;
        bst_init(&bst);

        struct bst_node node;

        bst_node_init(&node, 42);
        bst_insert_node(&bst, &node);

        assert(bst_contains_key(&bst, 42));
        assert(!bst_contains_key(&bst, 69));

        assert(bst == &node);
        assert(!bst_is_empty(&bst));

        assert(is_valid_red_black_tree(bst));
    }

    // N = 15, insert_node * 8, delete_node * 4, insert_node * 11
    {
        struct bst_node *bst;
        bst_init(&bst);

        struct bst_node node[8];
        const int values[8] = {3, 1, 5, 2, 4, 7, 6, 8};
        for (int i = 0; i < 8; i++) {
            bst_node_init(&node[i], values[i]);
            bst_insert_node(&bst, &node[i]);

            assert(is_valid_red_black_tree(bst));
        }

        const int deleted_values[4] = {3, 2, 4, 8};
        for (int i = 0; i < 4; i++) {
            bst_delete_node(&bst, bst_search_node(&bst, deleted_values[i]));

            assert(is_valid_red_black_tree(bst));
        }

        struct bst_node new_node[11];
        const int new_values[11] = {4, 12, 9, 10, 11, 16, 13, 14, 17, 15, 8};
        for (int i = 0; i < 11; i++) {
            bst_node_init(&new_node[i], new_values[i]);
            bst_insert_node(&bst, &new_node[i]);

            assert(is_valid_red_black_tree(bst));
        }
        assert(count_height(bst) == 4);
    }

    const int lim = 1024;

    // N = 8192, (4 * 2) * (insert_node * 1024, delete_node * 1024,
    //                      insert_node * 1024)
    for (int seed = 0; seed < 4; seed++) {
        struct bst_node *bst;
        bst_init(&bst);

        srand((unsigned int)seed);

        struct extended_bst_node *node_buf = calloc((size_t)(lim * 2), sizeof(struct extended_bst_node));
        size_t node_count = 0;

        for (int i = 0; i < lim; i++) {
            const int val = (int)((unsigned int)rand()) % lim;

            struct bst_node *node_ptr = bst_search_node(&bst, val);

            if (!node_ptr) {
                node_buf[node_count].value = 0;
                bst_node_init(&node_buf[node_count].node, val);
                bst_insert_node(&bst, &node_buf[node_count].node);
                node_count++;
            }
            else {
                container_of(node_ptr, struct extended_bst_node, node)->value++;
            }

            assert(is_valid_red_black_tree(bst));
        }

        assert(count_height(bst) <= 2 * log2(lim));

        for (int i = 0; i < lim; i++) {
            const int val = (int)((unsigned int)rand()) % lim;

            struct bst_node *node_ptr = bst_search_node(&bst, val);
            if (node_ptr) {
                unsigned int *value_ptr = &container_of(node_ptr, struct extended_bst_node, node)->value;
                if (*value_ptr > 1) {
                    (*value_ptr)--;
                }
                else {
                    bst_delete_node(&bst, node_ptr);
                }
            }
            assert(is_valid_red_black_tree(bst));
        }

        for (int i = 0; i < lim; i++) {
            const int val = (int)((unsigned int)rand()) % lim;

            struct bst_node *node_ptr = bst_search_node(&bst, val);

            if (!node_ptr) {
                node_buf[node_count].value = 0;
                bst_node_init(&node_buf[node_count].node, val);
                bst_insert_node(&bst, &node_buf[node_count].node);
                node_count++;
            }
            else {
                container_of(node_ptr, struct extended_bst_node, node)->value++;
            }
            assert(is_valid_red_black_tree(bst));
        }

        free(node_buf);
    }

    for (int seed = 4; seed < 8; seed++) {
        struct bstd_node *bstd;
        bstd_init(&bstd);

        srand((unsigned int)seed);

        struct bstd_node *node_buf = calloc((size_t)(lim * 2), sizeof(struct bstd_node));
        size_t node_count = 0;

        for (int i = 0; i < lim; i++) {
            const int val = (int)((unsigned int)rand()) % lim;

            bstd_node_init(&node_buf[node_count], val);
            bstd_insert_node(&bstd, &node_buf[node_count]);
            node_count++;

            assert(is_valid_red_black_tree_w_duplicates((struct bst_node *)bstd));
        }

        assert(count_height((struct bst_node *)bstd) <= 2 * log2(lim));

        for (int i = 0; i < lim; i++) {
            const int val = (int)((unsigned int)rand()) % lim;
            struct bstd_node *node_ptr = bstd_search_node(&bstd, val);
            if (node_ptr) {
                bstd_delete_node(&bstd, node_ptr);
            }
            assert(is_valid_red_black_tree_w_duplicates((struct bst_node *)bstd));
        }

        for (int i = 0; i < lim; i++) {
            const int val = (int)((unsigned int)rand()) % lim;

            bstd_node_init(&node_buf[node_count], val);
            bstd_insert_node(&bstd, &node_buf[node_count]);
            node_count++;

            assert(is_valid_red_black_tree_w_duplicates((struct bst_node *)bstd));
        }

        free(node_buf);
    }
}

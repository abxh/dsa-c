/*
    Arena struct should be opaque if the library is made non-header-only. So arena's struct member are not tested.
    Neither is the temporary arena functions, since the functions contain no branches and are simple.

    Mutating operation types:
    - init
    - deallocate_all
    - allocate_aligned / allocate
    - reallocate_aligned / reallocate

    Branches:
    - allocate_aligned()
        | !has_space_left -> NULL
        | otherwise -> (non-NULL pointer to chunk of initialized memory with correct alignment)
    - reallocate_aligned()
        | new_size == 0 || old_ptr == NULL || new_size || !inside_arena_buf -> NULL
        | has_optimized_w_prev_buf -> (same pointer, but interally shrinks/grows the memory chunk)
        | otherwise -> (new chunk of memory with previous buffer copied into with correct alignment)
*/

#define NAME arena
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "arena_template.h"

#include <string.h>

#include <stdio.h>

int main(void)
{
    // checking NULL cases for reallocate_aligned():
    {
        struct arena a;
        unsigned char buf[sizeof(max_align_t)];
        arena_init(&a, sizeof(buf), buf);

        assert(!arena_reallocate_aligned(&a, NULL, 1, 0, 0));
        assert(!arena_reallocate_aligned(&a, &buf[0], 1, 0, 1));
        assert(!arena_reallocate_aligned(&a, &buf[0], 1, 1, 0));
        assert(!arena_reallocate_aligned(&a, &a, 1, 1, 1));
    }
    // reallocate with same size + check NULL cases for allocate_aligned():
    {
        struct arena a;
        unsigned char buf[sizeof(max_align_t)];
        arena_init(&a, sizeof(buf), buf);

        unsigned char *p = arena_allocate_aligned(&a, alignof(max_align_t), sizeof(max_align_t));
        assert(p != NULL);

        for (size_t i = 0; i < sizeof(max_align_t); i++) {
            p[i] = 'a' + (unsigned char)i;
        }
        arena_reallocate_aligned(&a, p, alignof(max_align_t), sizeof(max_align_t), sizeof(max_align_t));
        for (size_t i = 0; i < sizeof(max_align_t); i++) {
            assert('a' + (char)i);
        }
        assert(!arena_allocate_aligned(&a, 1, 1));
    }
    // check if optimization using previous buffer works on an "used" arena --- grown chunk case:
    {
        struct arena a;
        unsigned char buf[16];
        arena_init(&a, sizeof(buf), buf);
        char *ptrs[8] = {0};
        for (size_t i = 0; i < 8; i++) {
            ptrs[i] = arena_allocate_aligned(&a, 1, 1);
            *ptrs[i] = 'a' + (char)i;
        }
        char *ptr = arena_allocate_aligned(&a, 1, 2);
        ptr[0] = 'z';
        ptr[1] = 'y';
        arena_reallocate_aligned(&a, ptr, 1, 2, 4);
        ptr[2] = 'x';
        ptr[3] = 'w';
        char *ptr2 = arena_allocate_aligned(&a, 1, 4);
        memcpy(ptr2, "aaaa", 4);

        assert(memcmp(ptr, "zyxw", 4) == 0);
        assert(!arena_allocate_aligned(&a, 1, 1));
    }
    // check if optimization using previous buffer works on an "used" arena --- shrunk chunk case:
    {
        struct arena a;
        unsigned char buf[16];
        arena_init(&a, sizeof(buf), buf);
        char *ptrs[8] = {0};
        for (size_t i = 0; i < 8; i++) {
            ptrs[i] = arena_allocate_aligned(&a, 1, 1);
            *ptrs[i] = 'a' + (char)i;
        }
        char *ptr = arena_allocate_aligned(&a, 1, 4);
        ptr[0] = 'z';
        ptr[1] = 'y';
        ptr[2] = 'x';
        ptr[3] = 'w';
        arena_reallocate_aligned(&a, ptr, 1, 2, 2);
        char *ptr2 = arena_allocate_aligned(&a, 1, 6);
        memcpy(ptr2, "aaaaaa", 6);

        assert(memcmp(ptr, "zy", 2) == 0);
        assert(!arena_allocate_aligned(&a, 1, 1));
    }
    // check when memory is copied:
    {
        struct arena a;
        unsigned char buf[16];
        arena_init(&a, sizeof(buf), buf);
        char *ptrs[8] = {0};
        for (size_t i = 0; i < 8; i++) {
            ptrs[i] = arena_allocate_aligned(&a, 1, 1);
            *ptrs[i] = 'a' + (char)i;
        }
        char *ptr = arena_allocate_aligned(&a, 1, 2);
        ptr[0] = 'z';
        ptr[1] = 'y';
        char *ptr2 = arena_allocate_aligned(&a, 1, 4);
        memcpy(ptr2, "mnop", 4);
        ptr = arena_reallocate_aligned(&a, ptr, 1, 2, 2);
        assert(memcmp(ptr2, "mnop", 4) == 0);
        assert(memcmp(ptr, "zy", 2) == 0);
        assert(!arena_allocate_aligned(&a, 1, 1));
    }
    // initialize with unaligned buffer:
    {
        struct arena a;
        unsigned char buf[1 + calc_alignment_padding(alignof(max_align_t), (uintptr_t)1) + sizeof(int)];
        arena_init(&a, sizeof(buf) - 1, &buf[1]);

        int *i1 = arena_allocate_aligned(&a, alignof(int), sizeof(int));
        *i1 = 1; // let the memory checker check this

        assert(!arena_allocate_aligned(&a, 1, 1));
    }
    // bad alignment test:
    {
        struct arena a;
        unsigned char buf[sizeof(int) * 4];

        arena_init(&a, sizeof(buf), buf);

        (void)(arena_allocate_aligned(&a, 1, 1));
        int *i1 = arena_allocate_aligned(&a, alignof(int), sizeof(int));
        *i1 = 1;

        (void)(arena_allocate_aligned(&a, 2, 2));
        int *i2 = arena_allocate_aligned(&a, alignof(int), sizeof(int));
        *i2 = 2;

        assert(!arena_allocate_aligned(&a, 1, 1));
    }
    // bad alignment test with rellocate():
    {
        struct arena a;
        unsigned char buf[sizeof(int) * 5];

        arena_init(&a, sizeof(buf), buf);

        (void)(arena_allocate_aligned(&a, 1, 1));
        int *i1 = arena_allocate_aligned(&a, alignof(int), sizeof(int));
        *i1 = 1;

        (void)(arena_allocate_aligned(&a, 2, 2));
        int *i2 = arena_reallocate_aligned(&a, i1, alignof(int), sizeof(int), sizeof(int) * 2);
        assert(*i1 == 1);
        *i2 = 2;

        assert(!arena_allocate_aligned(&a, 1, 1));
    }
}

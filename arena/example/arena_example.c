
#define NAME arena
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "arena.h"

#include <stdalign.h>
#include <stddef.h>
#include <string.h>

int main(void)
{
    unsigned char *buf = malloc(4096);

    struct arena arena;
    arena_init(&arena, 4096, buf);

    {
        struct arena_state arena_state = arena_state_save(&arena);

        char *str0 = arena_allocate_aligned(&arena, alignof(char), sizeof("A"));
        if (!str0) {
            assert(false);
        }

        str0[0] = 'a';
        str0[1] = '\0';
        assert(strcmp(str0, "a") == 0);

        void *tmp0 = arena_reallocate_aligned(&arena, str0, alignof(char), sizeof("A"), sizeof("AA"));
        if (!tmp0) {
            assert(false);
        }
        str0 = tmp0;

        assert(strcmp(str0, "a") == 0);
        strncpy(str0, "ab", sizeof("AA"));
        assert(strcmp(str0, "ab") == 0);

        char *str1 = arena_allocate(&arena, sizeof("cc"));
        if (!str1) {
            assert(false);
        }
        strncpy(str1, "cc", sizeof("AA"));

        assert(strcmp(str1, "cc") == 0);

        void *tmp1 = arena_reallocate(&arena, str0, sizeof("AA"), sizeof("AAAA"));
        if (!tmp1) {
            assert(false);
        }
        str0 = tmp1;

        assert(strcmp(str0, "ab") == 0);
        strncpy(str0, "abab", sizeof("AAAA"));
        assert(strcmp(str0, "abab") == 0);

        assert(strcmp(str1, "cc") == 0);

        assert(strcmp((char *)buf, "ab") == 0);

        assert(arena.curr_offset != 0);
        arena_state_restore(arena_state);
    }
    assert(arena.curr_offset == 0);

    arena_deallocate_all(&arena);

    free(buf);
}

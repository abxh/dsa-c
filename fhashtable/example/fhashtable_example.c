#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strint_ht.h"

static char buf[4096]; /* a static / heap-allocated buffer should be used, should the
                          key/values's lifetime extend beyond the function scope */

void str_int_ht_test_alt(void)
{
    struct strint_ht *ht = strint_ht_create(3);
    if (!ht) {
        assert(false);
    }
    assert(ht->count == 0);

    size_t buf_offset = 0;

    strcpy(&buf[buf_offset], "egg");
    char *egg_str_p = &buf[buf_offset];
    buf_offset += sizeof("egg");

    strint_ht_insert(ht, egg_str_p, 1);
    strint_ht_update(ht, egg_str_p, 2);

    strcpy(&buf[buf_offset], "milk");
    char *milk_str_p = &buf[buf_offset];
    buf_offset += sizeof("milk");

    strint_ht_update(ht, milk_str_p, 3);

    assert(ht->count == 2);

    assert(strint_ht_contains_key(ht, "milk"));
    assert(!strint_ht_contains_key(ht, "chocolate"));

    assert(strint_ht_get_value(ht, "egg", -1) == 2);

    int *res = strint_ht_get_value_mut(ht, "milk");
    if (res == NULL) {
        assert(false);
    }
    assert(*res == 3);
    *res = 4;

    assert(strint_ht_get_value(ht, "milk", -1) == 4);

    assert(strint_ht_delete(ht, "milk"));
    assert(!strint_ht_delete(ht, "chocolate"));

    assert(!strint_ht_contains_key(ht, "milk"));

    assert(strint_ht_search(ht, "milk") == NULL); // alias to get_mut

    assert(ht->count == 1);
    strint_ht_clear(ht);
    assert(ht->count == 0);

    assert(!strint_ht_contains_key(ht, "egg"));

    strint_ht_destroy(ht);
}

#include "murmurhash.h"

#define NAME               int_to_int_hashtable
#define KEY_TYPE           int
#define VALUE_TYPE         int
#define KEY_IS_EQUAL(a, b) ((a) == (b))
#define HASH_FUNCTION(key) (murmur3_32((uint8_t *)&(key), sizeof(int), 0))
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fhashtable_template.h"

#define LIM ((int)(1e+6))

void int_to_int_hashtable_test(void)
{
    struct int_to_int_hashtable *ht =
        int_to_int_hashtable_create(LIM * 4 / 3); // (4/3) scaling factor is to ensure hashtable
                                                  // performance by reducing maximum laod
    if (!ht) {
        assert(false);
    }
    for (int i = 0; i < LIM; i++) {
        int_to_int_hashtable_insert(ht, i, LIM - i);
    }
    for (int i = 0; i < LIM; i++) {
        assert(int_to_int_hashtable_get_value(ht, i, -1) == LIM - i);
    }

    struct int_to_int_hashtable *ht_copy = int_to_int_hashtable_create(ht->capacity);
    if (!ht_copy) {
        assert(false);
    }
    int_to_int_hashtable_copy(ht_copy, ht);

    int_to_int_hashtable_destroy(ht);

    bool table[LIM] = {false};
    {
        int key;
        int value;
        (void)(value); // mark unused

        size_t tempi;
        FHASHTABLE_FOR_EACH(ht_copy, tempi, key, value)
        {
            const bool inside_range = 0 <= key && key < LIM;
            const bool already_counted = table[key];

            if (!inside_range || already_counted) {
                assert(false);
            }

            table[key] = true;
        }
    }

    for (size_t i = 0; i < LIM; i++) {
        assert(table[i] == true);
    }

    assert(ht_copy->count == (size_t)LIM);
    int_to_int_hashtable_clear(ht_copy);
    assert(ht_copy->count == 0);

    int_to_int_hashtable_destroy(ht_copy);
}

int main(void)
{
    str_int_ht_test_alt();
    int_to_int_hashtable_test();
    return 0;
}

#include <assert.h>

#include "int_stack.h"

void int_stack_example(void)
{
    struct int_stack *s = int_stack_create(3);

    if (!s) {
        assert(false);
    }

    int_stack_is_empty(s);

    assert(int_stack_is_empty(s));
    assert(!int_stack_is_full(s));

    int_stack_push(s, 1);
    int_stack_push(s, 2);
    int_stack_push(s, 3);

    assert(!int_stack_is_empty(s));
    assert(int_stack_is_full(s));

    assert(s->count == 3);

    assert(int_stack_get_top(s) == 3);
    assert(int_stack_get_bottom(s) == 1);

    assert(int_stack_peek(s) == 3);

    assert(int_stack_at(s, 0) == 3);
    assert(int_stack_at(s, 1) == 2);
    assert(int_stack_at(s, 2) == 1);

    const int res = int_stack_pop(s);
    assert(res == 3);

    assert(s->count == 2);

    int_stack_clear(s);

    assert(s->count == 0);

    int_stack_destroy(s);
}

#define NAME       uintstk
#define VALUE_TYPE unsigned int
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fstack_template.h"

void uintstk_example(void)
{
    const size_t lim = 1e+6;

    struct uintstk *s = uintstk_create((uint32_t)lim);
    if (!s) {
        assert(false);
    }

    for (unsigned int i = 1; i <= lim; i++) {
        uintstk_push(s, i);
    }

    struct uintstk *s_copy = uintstk_create(s->capacity);
    if (!s_copy) {
        assert(false);
    }
    uintstk_copy(s_copy, s);

    for (unsigned int i = (unsigned int)lim; i >= 1; i--) {
        assert(i == uintstk_pop(s));
    }

    {
        size_t x = 1;
        unsigned int value;

        size_t tempi;
        FSTACK_FOR_EACH_REVERSE(s_copy, tempi, value)
        {
            assert(value == x++);
        }
    }

    {
        size_t cpy_count = 0;
        size_t x = lim;
        unsigned int value;

        size_t tempi;
        FSTACK_FOR_EACH(s_copy, tempi, value)
        {
            assert(value == x--);
            cpy_count++;
        }
        assert(cpy_count == lim);
    }

    assert(s_copy->count == lim);
    uintstk_clear(s_copy);
    assert(s_copy->count == 0);

    uintstk_destroy(s);
    uintstk_destroy(s_copy);
}

int main(void)
{
    int_stack_example();
    uintstk_example();
}

#include <assert.h>
#include <stdbool.h>

#include "char_queue.h"

void char_queue_example(void)
{
    struct char_queue *q = char_queue_create(4);
    if (!q) {
        assert(false);
    }
    char_queue_is_empty(q);

    assert(char_queue_is_empty(q));
    assert(!char_queue_is_full(q));

    char_queue_enqueue(q, 'a');
    char_queue_enqueue(q, 'b');
    char_queue_enqueue(q, 'c');
    char_queue_enqueue(q, 'd');

    assert(!char_queue_is_empty(q));
    assert(char_queue_is_full(q)); // note: capacity is rounded up to the next power of 2.

    assert(q->count == 4);

    assert('a' == char_queue_get_front(q));
    assert('d' == char_queue_get_back(q));

    assert('a' == char_queue_peek(q));

    assert('a' == char_queue_at(q, 0));
    assert('b' == char_queue_at(q, 1));
    assert('c' == char_queue_at(q, 2));
    assert('d' == char_queue_at(q, 3));

    const char res = char_queue_dequeue(q);
    assert('a' == res);

    assert(q->count == 3);

    char_queue_clear(q);

    assert(q->count == 0);

    char_queue_destroy(q);
}

#define NAME       int_queue
#define VALUE_TYPE int
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fqueue_template.h"

void int_queue_example()
{
    const int lim = 1e+6;
    struct int_queue *q = int_queue_create((uint32_t)lim);

    if (!q) {
        assert(false);
    }

    for (int i = 1; i <= lim; i++) {
        int_queue_enqueue(q, i);
    }

    struct int_queue *q_copy = int_queue_create((uint32_t)lim);
    if (!q_copy) {
        assert(false);
    }
    int_queue_copy(q_copy, q);

    for (int i = 1; i <= lim; i++) {
        assert(i == int_queue_dequeue(q));
    }

    {
        int x = lim;
        int value;

        size_t tempi;
        FQUEUE_FOR_EACH_REVERSE(q_copy, tempi, value)
        {
            assert(value == x--);
        }
    }
    {
        int x = 1;
        int count = 0;
        int value;

        size_t tempi;
        FQUEUE_FOR_EACH(q_copy, tempi, value)
        {
            assert(value == x++);
            count++;
        }
    }

    assert(q_copy->count == (size_t)lim);
    int_queue_clear(q_copy);
    assert(q_copy->count == 0);

    int_queue_destroy(q);
    int_queue_destroy(q_copy);
}

int main(void)
{
    char_queue_example();
    int_queue_example();
    return 0;
}

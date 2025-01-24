/*
    Test cases (N):
    - N := 0
    - N := 1
    - N := 2
    - N := 10
    - N := 1e+6

    Non-mutating operation types / properties:
    - .count
    - .capacity
    - is_empty
    - is_full
    - get_max / peek
    - fpqueue_for_each
    - calc_sizeof (this is indirectly tested for with `create`)
    - fpqueue_left_child + fpqueue_right_child + fpqueue_parent

    Mutating operation types:
    - push
    - pop_max
    - clear

    Memory operations [to also be tested with sanitizers]:
    - init (this is indirectly tested for with `create`)
    - create
    - destroy
    - copy
*/

#define NAME       i64_pque
#define VALUE_TYPE int64_t
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fpqueue_template.h"

static inline bool check_count_invariance(const struct i64_pque *que_p, const size_t push_op_count,
                                          const size_t pop_op_count)
{
    return que_p->count == (push_op_count - pop_op_count);
}

static inline bool check_capacity_invariance(const struct i64_pque *que_p, const size_t suggested_capacity)
{
    return que_p->capacity >= suggested_capacity;
}

static inline bool check_empty_full(const struct i64_pque *que_p, const size_t push_op_count, const size_t pop_op_count)
{
    const size_t diff = push_op_count - pop_op_count;

    if (i64_pque_is_empty(que_p)) {
        return diff == 0;
    }
    else if (i64_pque_is_full(que_p)) {
        return diff == que_p->capacity;
    }
    else {
        return true;
    }
}

static inline bool check_heap_property(const struct i64_pque *que_p, const uint32_t index)
{
    if (index < que_p->count) {
        return true;
    }
    bool res = true;

    const uint32_t l = FPQUEUE_LEFT_CHILD(index);
    const uint32_t r = FPQUEUE_RIGHT_CHILD(index);

    if (l < que_p->count) {
        res &= que_p->elements[l].priority <= que_p->elements[index].priority;
    }
    if (r < que_p->count) {
        res &= que_p->elements[r].priority <= que_p->elements[index].priority;
    }
    return res && check_heap_property(que_p, l) && check_heap_property(que_p, r);
}

static inline bool check_priority_queue_property(const struct i64_pque *que_p, const int64_t suggest_max_priority_value)
{
    if (que_p->count == 0) {
        return true;
    }

    uint32_t max_priority = 0;
    {
        int64_t value;
        (void)(value);

        uint32_t index;
        FPQUEUE_FOR_EACH(que_p, index, value)
        {
            uint32_t curr = que_p->elements[index].priority;
            max_priority = curr > max_priority ? curr : max_priority;
        }
    }
    return max_priority == suggest_max_priority_value;
}

static inline bool copy_values_and_check_ordered_values(const struct i64_pque *que_p)
{
    if (que_p->count == 0) {
        return true;
    }

    struct i64_pque *que_copy_p = i64_pque_create((uint32_t)que_p->count);
    if (!que_copy_p) {
        return false;
    }
    i64_pque_copy(que_copy_p, que_p);

    uint32_t max_priority = UINT32_MAX;
    bool res = true;
    while (!i64_pque_is_empty(que_copy_p)) {
        const uint32_t curr = que_copy_p->elements[0].priority;
        assert(max_priority >= curr);
        max_priority = curr;
        i64_pque_pop_max(que_copy_p);
    }

    i64_pque_destroy(que_copy_p);
    return res;
}

int main(void)
{
    // N = 0
    {
        struct i64_pque *que_p = i64_pque_create(0);
        if (que_p) {
            assert(false);
        }
    }
    // N = 1
    {
        struct i64_pque *que_p = i64_pque_create(1);
        if (!que_p) {
            assert(false);
        }

        assert(check_count_invariance(que_p, 0, 0));
        assert(check_capacity_invariance(que_p, 1));
        assert(check_empty_full(que_p, 0, 0));

        i64_pque_destroy(que_p);
    }
    // N = 1, push
    {
        struct i64_pque *que_p = i64_pque_create(1);
        if (!que_p) {
            assert(false);
        }
        i64_pque_push(que_p, 69, 42);

        assert(check_count_invariance(que_p, 1, 0));
        assert(check_capacity_invariance(que_p, 1));
        assert(check_empty_full(que_p, 1, 0));
        assert(i64_pque_peek(que_p) == 69);
        assert(check_priority_queue_property(que_p, 42));
        assert(check_heap_property(que_p, 0));

        i64_pque_destroy(que_p);
    }
    // N = 1, push -> pop_max
    {
        struct i64_pque *que_p = i64_pque_create(1);
        if (!que_p) {
            assert(false);
        }
        i64_pque_push(que_p, 69, 42);

        assert(i64_pque_pop_max(que_p) == 69);
        assert(check_count_invariance(que_p, 1, 1));
        assert(check_capacity_invariance(que_p, 1));
        assert(check_empty_full(que_p, 1, 1));

        i64_pque_destroy(que_p);
    }
    // N = 2, push * 2
    {
        struct i64_pque *que_p = i64_pque_create(2);
        if (!que_p) {
            assert(false);
        }
        i64_pque_push(que_p, 69, 42);
        i64_pque_push(que_p, 42, 69);

        assert(i64_pque_peek(que_p) == 42);
        assert(check_count_invariance(que_p, 2, 0));
        assert(check_capacity_invariance(que_p, 2));
        assert(check_empty_full(que_p, 2, 0));
        assert(check_priority_queue_property(que_p, 69));
        assert(check_heap_property(que_p, 0));
        assert(copy_values_and_check_ordered_values(que_p));

        i64_pque_destroy(que_p);
    }
    // N = 10, push * 10
    {
        struct i64_pque *que_p = i64_pque_create(10);
        if (!que_p) {
            assert(false);
        }
        i64_pque_push(que_p, 421, 10);
        i64_pque_push(que_p, 422, 9);
        i64_pque_push(que_p, 423, 8);
        i64_pque_push(que_p, 424, 7);
        i64_pque_push(que_p, 425, 6);
        i64_pque_push(que_p, 426, 5);
        i64_pque_push(que_p, 427, 4);
        i64_pque_push(que_p, 428, 3);
        i64_pque_push(que_p, 429, 2);
        i64_pque_push(que_p, 430, 1);

        assert(i64_pque_peek(que_p) == 421);
        assert(check_count_invariance(que_p, 10, 0));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 10, 0));
        assert(check_priority_queue_property(que_p, 10));
        assert(check_heap_property(que_p, 0));
        assert(copy_values_and_check_ordered_values(que_p));

        i64_pque_destroy(que_p);
    }
    // N = 10, push * 10 -> pop_max * 5
    {
        struct i64_pque *que_p = i64_pque_create(10);
        if (!que_p) {
            assert(false);
        }
        i64_pque_push(que_p, 421, 10);
        i64_pque_push(que_p, 422, 9);
        i64_pque_push(que_p, 423, 8);
        i64_pque_push(que_p, 424, 7);
        i64_pque_push(que_p, 425, 6);
        i64_pque_push(que_p, 426, 5);
        i64_pque_push(que_p, 427, 4);
        i64_pque_push(que_p, 428, 3);
        i64_pque_push(que_p, 429, 2);
        i64_pque_push(que_p, 430, 1);

        assert(i64_pque_pop_max(que_p) == 421);
        assert(i64_pque_pop_max(que_p) == 422);
        assert(i64_pque_pop_max(que_p) == 423);
        assert(i64_pque_pop_max(que_p) == 424);
        assert(i64_pque_pop_max(que_p) == 425);

        assert(i64_pque_peek(que_p) == 426);
        assert(check_count_invariance(que_p, 10, 5));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 10, 5));
        assert(check_priority_queue_property(que_p, 5));
        assert(check_heap_property(que_p, 0));
        assert(copy_values_and_check_ordered_values(que_p));

        i64_pque_destroy(que_p);
    }
    // N = 10, push * 10 -> clear() -> push * 5
    {
        struct i64_pque *que_p = i64_pque_create(10);
        if (!que_p) {
            assert(false);
        }
        for (size_t i = 0; i < 10; i++) {
            i64_pque_push(que_p, 421 + (int)i, (uint32_t)i + 1);
        }
        i64_pque_clear(que_p);

        i64_pque_push(que_p, 436, 6);
        i64_pque_push(que_p, 437, 7);
        i64_pque_push(que_p, 438, 8);
        i64_pque_push(que_p, 439, 9);
        i64_pque_push(que_p, 440, 10);

        assert(i64_pque_peek(que_p) == 440);
        assert(check_count_invariance(que_p, 5, 0));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 10, 0));
        assert(check_priority_queue_property(que_p, 10));
        assert(check_heap_property(que_p, 0));
        assert(copy_values_and_check_ordered_values(que_p));

        i64_pque_destroy(que_p);
    }
    // N = 1e+6, push * 1e+6 -> pop_max * 1e+3
    {
        struct i64_pque *que_p = i64_pque_create(1e+6);
        if (!que_p) {
            assert(false);
        }
        for (size_t i = 0; i < 1e+6; i++) {
            i64_pque_push(que_p, 420 + (int)i, (uint32_t)1e+6 - (uint32_t)i);
        }
        for (size_t i = 0; i < 1e+3; i++) {
            assert(i64_pque_pop_max(que_p) == (int64_t)(420 + i));
        }
        assert(i64_pque_peek(que_p) == 420 + (int)1e+3);
        assert(check_count_invariance(que_p, 1e+6, 1e+3));
        assert(check_capacity_invariance(que_p, 1e+6));
        assert(check_empty_full(que_p, 1e+6, 1e+3));
        assert(check_priority_queue_property(que_p, (uint32_t)1e+6 - (uint32_t)1e+3));
        assert(check_heap_property(que_p, 0));
        assert(copy_values_and_check_ordered_values(que_p));

        i64_pque_destroy(que_p);
    }
}

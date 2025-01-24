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
    - get_front / peek
    - get_back
    - at + fqueue_for_each + fqueue_for_each_reverse
    - calc_sizeof (this is indirectly tested for with `create`)

    Mutating operation types:
    - enqueue
    - dequeue
    - clear

    Memory operations [to also be tested with sanitizers]:
    - init (this is indirectly tested for with `create`)
    - create
    - destroy
    - copy
*/

#define NAME       i64_que
#define VALUE_TYPE int64_t
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fqueue_template.h"

static inline bool check_count_invariance(const struct i64_que *que_p, const size_t enqueue_op_count,
                                          const size_t dequeue_op_count)
{
    // enqueue and dequeue operations *after* the stack is cleared / created
    return que_p->count == (enqueue_op_count - dequeue_op_count);
}

static inline bool check_capacity_invariance(const struct i64_que *que_p, const size_t suggested_capacity)
{
    return que_p->capacity >= suggested_capacity;
}

static inline bool check_empty_full(const struct i64_que *que_p, const size_t enqueue_op_count,
                                    const size_t dequeue_op_count)
{
    // enqueue and dequeue operations *after* the stack is cleared / created
    const size_t diff = enqueue_op_count - dequeue_op_count;

    if (i64_que_is_empty(que_p)) {
        return diff == 0;
    }
    else if (i64_que_is_full(que_p)) {
        return diff == que_p->capacity;
    }
    else {
        return true;
    }
}

static inline bool check_front_back(const struct i64_que *que_p, const int64_t latest_value_enqueueed,
                                    const int64_t first_value_enqueueed)
{
    // first and last value enqueueed *after* the stack is cleared / created
    return i64_que_get_front(que_p) == latest_value_enqueueed && i64_que_get_back(que_p) == first_value_enqueueed;
}

static inline bool check_ordered_values(const struct i64_que *que_p, const size_t n, const int64_t expected_value[n])
{
    assert(n != 0);

    bool res = true;
    for (size_t i = 0; i < n; i++) {
        res &= i64_que_at(que_p, (uint32_t)i) == expected_value[i];
    }
    {
        size_t index = 0;
        int64_t value;

        size_t tempi;
        FQUEUE_FOR_EACH(que_p, tempi, value)
        {
            res &= value == expected_value[index++];
        }
        assert(index == n);
    }
    {
        size_t index = n;
        int64_t value;

        size_t tempi;
        FQUEUE_FOR_EACH_REVERSE(que_p, tempi, value)
        {
            res &= value == expected_value[--index];
        }
        assert(index == 0);
    }
    return res;
}

static inline bool copy_values_and_check_ordered_values(const struct i64_que *que_p, const size_t n,
                                                        const int64_t expected_value[n])
{
    assert(n != 0);

    struct i64_que *que_copy_p = i64_que_create((uint32_t)n);
    if (!que_copy_p) {
        return false;
    }
    i64_que_copy(que_copy_p, que_p);
    const bool res = check_ordered_values(que_p, n, expected_value);
    i64_que_destroy(que_copy_p);
    return res;
}

int main(void)
{
    // N = 0
    {
        struct i64_que *que_p = i64_que_create(0);
        if (que_p) {
            assert(false);
        }
    }
    // N = 1
    {
        struct i64_que *que_p = i64_que_create(1);
        if (!que_p) {
            assert(false);
        }

        assert(check_count_invariance(que_p, 0, 0));
        assert(check_capacity_invariance(que_p, 1));
        assert(check_empty_full(que_p, 0, 0));

        i64_que_destroy(que_p);
    }
    // N = 1, enqueue
    {
        struct i64_que *que_p = i64_que_create(1);
        if (!que_p) {
            assert(false);
        }
        i64_que_enqueue(que_p, 42);

        assert(check_count_invariance(que_p, 1, 0));
        assert(check_capacity_invariance(que_p, 1));
        assert(check_empty_full(que_p, 1, 0));
        assert(check_front_back(que_p, 42, 42));

        i64_que_destroy(que_p);
    }
    // N = 1, enqueue -> dequeue
    {
        struct i64_que *que_p = i64_que_create(1);
        if (!que_p) {
            assert(false);
        }
        i64_que_enqueue(que_p, 2);

        assert(i64_que_dequeue(que_p) == 2);
        assert(check_count_invariance(que_p, 1, 1));
        assert(check_capacity_invariance(que_p, 1));
        assert(check_empty_full(que_p, 1, 1));

        i64_que_destroy(que_p);
    }
    // N = 2, enqueue * 2
    {
        struct i64_que *que_p = i64_que_create(2);
        if (!que_p) {
            assert(false);
        }
        i64_que_enqueue(que_p, 42);
        i64_que_enqueue(que_p, 69);

        assert(check_count_invariance(que_p, 2, 0));
        assert(check_capacity_invariance(que_p, 2));
        assert(check_empty_full(que_p, 2, 0));
        assert(check_front_back(que_p, 42, 69));
        assert(check_ordered_values(que_p, 2, (int64_t[2]){42, 69}));
        assert(copy_values_and_check_ordered_values(que_p, 2, (int64_t[2]){42, 69}));

        i64_que_destroy(que_p);
    }
    // N = 2, enqueue * 2 -> dequeue
    {
        struct i64_que *que_p = i64_que_create(2);
        if (!que_p) {
            assert(false);
        }
        i64_que_enqueue(que_p, 42);
        i64_que_enqueue(que_p, 69);
        assert(i64_que_dequeue(que_p) == 42);

        assert(check_count_invariance(que_p, 2, 1));
        assert(check_capacity_invariance(que_p, 2));
        assert(check_empty_full(que_p, 2, 1));
        assert(check_front_back(que_p, 69, 69));

        i64_que_destroy(que_p);
    }
    // N = 10, enqueue * 10
    {
        struct i64_que *que_p = i64_que_create(10);
        if (!que_p) {
            assert(false);
        }
        i64_que_enqueue(que_p, 421);
        i64_que_enqueue(que_p, 422);
        i64_que_enqueue(que_p, 423);
        i64_que_enqueue(que_p, 424);
        i64_que_enqueue(que_p, 425);
        i64_que_enqueue(que_p, 426);
        i64_que_enqueue(que_p, 427);
        i64_que_enqueue(que_p, 428);
        i64_que_enqueue(que_p, 429);
        i64_que_enqueue(que_p, 430);

        assert(check_count_invariance(que_p, 10, 0));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 10, 0));
        assert(check_front_back(que_p, 421, 430));
        assert(check_ordered_values(que_p, 10, (int64_t[10]){421, 422, 423, 424, 425, 426, 427, 428, 429, 430}));
        assert(copy_values_and_check_ordered_values(que_p, 10,
                                                    (int64_t[10]){421, 422, 423, 424, 425, 426, 427, 428, 429, 430}));

        i64_que_destroy(que_p);
    }
    // N = 10, enqueue * 10 -> dequeue * 5
    {
        struct i64_que *que_p = i64_que_create(10);
        if (!que_p) {
            assert(false);
        }
        i64_que_enqueue(que_p, 421);
        i64_que_enqueue(que_p, 422);
        i64_que_enqueue(que_p, 423);
        i64_que_enqueue(que_p, 424);
        i64_que_enqueue(que_p, 425);
        i64_que_enqueue(que_p, 426);
        i64_que_enqueue(que_p, 427);
        i64_que_enqueue(que_p, 428);
        i64_que_enqueue(que_p, 429);
        i64_que_enqueue(que_p, 430);

        assert(i64_que_dequeue(que_p) == 421);
        assert(i64_que_dequeue(que_p) == 422);
        assert(i64_que_dequeue(que_p) == 423);
        assert(i64_que_dequeue(que_p) == 424);
        assert(i64_que_dequeue(que_p) == 425);

        assert(check_count_invariance(que_p, 10, 5));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 10, 5));
        assert(check_front_back(que_p, 426, 430));
        assert(check_ordered_values(que_p, 5, (int64_t[5]){426, 427, 428, 429, 430}));
        assert(copy_values_and_check_ordered_values(que_p, 5, (int64_t[5]){426, 427, 428, 429, 430}));

        i64_que_destroy(que_p);
    }
    // N = 10, enqueue * 3, dequeue * 3, enqueue * 10 -> dequeue * 5 -> enqueue * 3
    {
        struct i64_que *que_p = i64_que_create(10);
        if (!que_p) {
            assert(false);
        }
        for (size_t i = 0; i < 5; i++) {
            i64_que_enqueue(que_p, (int)i);
        }
        for (size_t i = 0; i < 5; i++) {
            i64_que_dequeue(que_p);
        }
        i64_que_enqueue(que_p, 421);
        i64_que_enqueue(que_p, 422);
        i64_que_enqueue(que_p, 423);
        i64_que_enqueue(que_p, 424);
        i64_que_enqueue(que_p, 425);
        i64_que_enqueue(que_p, 426);
        i64_que_enqueue(que_p, 427);
        i64_que_enqueue(que_p, 428);
        i64_que_enqueue(que_p, 429);
        i64_que_enqueue(que_p, 430);

        for (size_t i = 0; i < 5; i++) {
            i64_que_dequeue(que_p);
        }

        i64_que_enqueue(que_p, 431);
        i64_que_enqueue(que_p, 432);

        assert(check_count_invariance(que_p, 12, 5));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 12, 5));
        assert(check_front_back(que_p, 426, 432));
        assert(check_ordered_values(que_p, 7, (int64_t[8]){426, 427, 428, 429, 430, 431, 432}));
        assert(copy_values_and_check_ordered_values(que_p, 7, (int64_t[7]){426, 427, 428, 429, 430, 431, 432}));

        i64_que_destroy(que_p);
    }
    // N = 10, enqueue * 10 -> clear() -> enqueue * 5
    {
        struct i64_que *que_p = i64_que_create(10);
        if (!que_p) {
            assert(false);
        }
        for (size_t i = 0; i < 10; i++) {
            i64_que_enqueue(que_p, 420 + (int)i);
        }
        i64_que_clear(que_p);

        i64_que_enqueue(que_p, 431);
        i64_que_enqueue(que_p, 432);
        i64_que_enqueue(que_p, 433);
        i64_que_enqueue(que_p, 434);
        i64_que_enqueue(que_p, 435);

        assert(check_count_invariance(que_p, 15 - 10, 0));
        assert(check_capacity_invariance(que_p, 10));
        assert(check_empty_full(que_p, 15 - 10, 0));
        assert(check_front_back(que_p, 431, 435));
        assert(check_ordered_values(que_p, 5, (int64_t[5]){431, 432, 433, 434, 435}));
        assert(copy_values_and_check_ordered_values(que_p, 5, (int64_t[10]){431, 432, 433, 434, 435}));

        i64_que_destroy(que_p);
    }
    // N = 1e+6, enqueue * 1e+6
    {
        struct i64_que *que_p = i64_que_create(1e+6);
        if (!que_p) {
            assert(false);
        }
        for (size_t i = 0; i < 1e+6; i++) {
            i64_que_enqueue(que_p, 420 + (int)i);
        }
        assert(check_count_invariance(que_p, 1e+6, 0));
        assert(check_capacity_invariance(que_p, 1e+6));
        assert(check_empty_full(que_p, 1e+6, 0));
        assert(check_front_back(que_p, 420, 420 + 1e+6 - 1));

        i64_que_destroy(que_p);
    }
    // N = 1e+6, enqueue * 1e+6 -> dequeue * 1e+3
    {
        struct i64_que *que_p = i64_que_create(1e+6);
        if (!que_p) {
            assert(false);
        }
        for (size_t i = 0; i < 1e+6; i++) {
            i64_que_enqueue(que_p, 420 + (int)i);
        }
        for (size_t i = 0; i < 1e+3; i++) {
            assert(i64_que_dequeue(que_p) == (int64_t)(420 + i));
        }
        assert(check_count_invariance(que_p, 1e+6, 1e+3));
        assert(check_capacity_invariance(que_p, 1e+6));
        assert(check_empty_full(que_p, 1e+6, 1e+3));
        assert(check_front_back(que_p, 420 + 1e+3, 420 + 1e+6 - 1));

        i64_que_destroy(que_p);
    }
}

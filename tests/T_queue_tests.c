#include <stdbool.h> // bool, true, false
#include <stdio.h>   // fprintf, printf, stderr

#include "test.h" // is_true, is_false, is_equal

#define VALUE_TYPE int
#include "header-only/fixed-containers/T_queue.h" // int_queue_*, T_queue_for_each

bool empty_test(void) {
    int_queue_type* queue_p = int_queue_create(1);
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;

    res &= is_equal(int_queue_get_count(queue_p), 0UL);
    res &= is_true(int_queue_is_empty(queue_p));

    int_queue_destroy(queue_p);

    return res;
}

bool one_element_test(void) {
    int_queue_type* queue_p = int_queue_create(1);
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    int value = 5;

    res &= is_true(int_queue_is_empty(queue_p));
    res &= is_false(int_queue_is_full(queue_p));

    int_queue_enqueue(queue_p, value);
    res &= is_false(int_queue_is_empty(queue_p));
    res &= is_equal(value, int_queue_peek(queue_p));
    res &= is_equal(value, int_queue_peek_first(queue_p));
    res &= is_equal(value, int_queue_peek_last(queue_p));

    res &= is_equal(value, int_queue_dequeue(queue_p));
    res &= is_true(int_queue_is_empty(queue_p));
    res &= is_false(int_queue_is_full(queue_p));

    int_queue_destroy(queue_p);

    return res;
}

bool two_elements_test(void) {
    int_queue_type* queue_p = int_queue_create(2);
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    int value1 = 5;
    int value2 = 10;

    res &= is_equal(int_queue_get_capacity(queue_p), 2UL);

    res &= is_true(int_queue_is_empty(queue_p));
    res &= is_false(int_queue_is_full(queue_p));

    int_queue_enqueue(queue_p, value1);
    res &= is_false(int_queue_is_empty(queue_p));
    res &= is_false(int_queue_is_full(queue_p));

    int_queue_enqueue(queue_p, value2);
    res &= is_false(int_queue_is_empty(queue_p));
    res &= is_true(int_queue_is_full(queue_p));

    res &= is_equal(value1, int_queue_peek_first(queue_p));
    res &= is_equal(value2, int_queue_peek_last(queue_p));

    res &= is_equal(value1, int_queue_dequeue(queue_p));
    res &= is_equal(value2, int_queue_peek_first(queue_p));
    res &= is_equal(value2, int_queue_peek_last(queue_p));

    int_queue_enqueue(queue_p, value1);
    res &= is_equal(value2, int_queue_peek_first(queue_p));
    res &= is_equal(value1, int_queue_peek_last(queue_p));

    int_queue_destroy(queue_p);

    return res;
}

bool million_elements_test(void) {
    int_queue_type* queue_p = int_queue_create(1000000);
    if (!queue_p) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        int_queue_enqueue(queue_p, i);
        res &= is_equal(int_queue_get_count(queue_p), (size_t)i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= is_equal(i, int_queue_dequeue(queue_p));
        res &= is_equal(int_queue_get_count(queue_p), (size_t)(1000000 - i));
    }
    int_queue_destroy(queue_p);
    return res;
}

bool wraparound_test(void) {
    int_queue_type* queue_p = int_queue_create(1024);
    if (!queue_p) {
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 750; i++) {
        int_queue_enqueue(queue_p, i);
    }
    for (int i = 1; i <= 750; i++) {
        res &= is_equal(i, int_queue_dequeue(queue_p));
    }
    for (int i = 1; i <= 750; i++) {
        int_queue_enqueue(queue_p, i);
    }
    for (int i = 1; i <= 750; i++) {
        res &= is_equal(i, int_queue_dequeue(queue_p));
    }
    int_queue_destroy(queue_p);
    return res;
}

bool for_each_and_copy_test(void) {
    int_queue_type* queue_p = int_queue_create(50);
    if (!queue_p) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        int_queue_enqueue(queue_p, i);
    }
    {
        int x = 51;
        int out_value;
        size_t out_index;
        T_queue_for_each(queue_p, out_index, out_value) {
            res &= is_equal(x, out_value);
            x++;
        }
    }
    int_queue_type* queue_copy_p;
    if (!int_queue_copy(&queue_copy_p, queue_p)) {
        fprintf(stderr, "could not initialize object in %s at line %d.\n", __PRETTY_FUNCTION__, __LINE__);
        return false;
    }
    {
        int x = 51;
        int out_value;
        size_t out_index;
        T_queue_for_each(queue_copy_p, out_index, out_value) {
            res &= is_equal(x, out_value);
            x++;
        }
    }

    int_queue_destroy(queue_p);
    int_queue_destroy(queue_copy_p);

    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} bool_f_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    bool_f_plus bool_f_arr[] = {{empty_test, "empty test"},
                                {one_element_test, "one element test"},
                                {two_elements_test, "two elements test"},
                                {million_elements_test, "million elements test"},
                                {wraparound_test, "wraparound test"},
                                {for_each_and_copy_test, "for each and copy test"}};
    printf(__FILE_NAME__ ":\n");
    bool overall_res = true;
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(bool_f_plus); i++) {
        bool res = bool_f_arr[i].func();
        overall_res &= res;
        printf(" [%s] %s\n", res ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return overall_res ? 0 : 1;
}

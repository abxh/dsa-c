#include <stdbool.h>
#include <stdio.h>

#include "eval_bool_and_log.h"
#define el(expr) eval_bool_and_log(expr)

#include "../src/queue/queue.h"
#define VALUE_TYPE int
#define VALUE_NAME int
#include "../src/queue/queue_helpers.h"

bool empty_test(void) {
    Queue* q;
    if (!queue_init_int(&q)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;

    res &= el(queue_count(q) == 0);
    res &= el(queue_is_empty(q));
    res &= el(queue_deinit(&q));

    return res;
}

bool one_element_test(void) {
    Queue* q;
    if (!queue_init_int(&q)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    int value = 5;

    res &= el(queue_enqueue_int(q, value));
    res &= el(value == queue_peek_int(q));

    res &= el(!queue_is_empty(q));
    res &= el(queue_count(q) == 1);

    res &= el(value == queue_dequeue_int(q));
    res &= el(queue_is_empty(q));
    res &= el(queue_count(q) == 0);

    res &= el(queue_deinit(&q));

    return res;
}

bool two_elements_test(void) {
    Queue* q;
    if (!queue_init_int(&q)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    int value1 = 1;
    int value2 = 2;

    res &= el(queue_enqueue_int(q, value1));
    res &= el(queue_enqueue_int(q, value2));
    res &= el(value1 == queue_peek_first_int(q));
    res &= el(value2 == queue_peek_last_int(q));
    res &= el(queue_count(q) == 2);
    res &= el(queue_deinit(&q));

    return res;
}

bool million_elements_test(void) {
    Queue* q;
    if (!queue_init_int(&q)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 1; i <= 1000000; i++) {
        queue_enqueue_int(q, i);
    }
    for (int i = 1; i <= 1000000; i++) {
        res &= el(i == queue_dequeue_int(q));
    }
    res &= el(queue_deinit(&q));
    return res;
}

bool for_each_test(void) {
    Queue* q;
    if (!queue_init_int(&q)) {
        fprintf(stderr, "could not initialize object in %s.\n", __PRETTY_FUNCTION__);
        return false;
    }
    bool res = true;
    for (int i = 51; i <= 100; i++) {
        queue_enqueue_int(q, i);
    }
    int x = 51;
    QueueNode* node_p;
    int value;
    queue_for_each(q, node_p, value) {
        res &= el(x == value);
        x++;
    }
    res &= el(queue_deinit(&q));
    return res;
}

typedef bool (*bool_f)(void);

typedef struct {
    bool_f func;
    const char* desc;
} func_plus;

#define OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"

int main(void) {
    func_plus bool_f_arr[] = {{empty_test, "empty test"},
                              {one_element_test, "one element test"},
                              {two_elements_test, "two elements test"},
                              {million_elements_test, "million elements test"},
                              {for_each_test, "for each test"}};
    printf(__FILE_NAME__ ":\n");
    for (size_t i = 0; i < sizeof(bool_f_arr) / sizeof(func_plus); i++) {
        printf(" [%s] %s\n", bool_f_arr[i].func() ? GREEN "true" OFF : RED "false" OFF, bool_f_arr[i].desc);
    }
    return 0;
}

#include <float.h>
#include <math.h>

#define NAME       flt_queue
#define VALUE_TYPE float
#define TYPE_DEFINITIONS
#define FUNCTION_DEFINITIONS
#define FUNCTION_LINKAGE static inline
#include "fqueue_template.h"

struct data_stream {
    float avg;
    struct flt_queue *queue;
};

static inline void data_stream_init(struct data_stream *s, uint32_t max_elm)
{
    s->avg = FP_NAN;
    s->queue = flt_queue_create(max_elm);
}

static inline void data_stream_deinit(struct data_stream *s)
{
    flt_queue_destroy(s->queue);
}

static inline void data_stream_enqueue(struct data_stream *s, float val)
{
    if (flt_queue_is_full(s->queue)) {
        return;
    }
    if (s->queue->count == 0) {
        s->avg = val;
    }
    else {
        s->avg = (s->avg * (float)s->queue->count + val) / (float)(s->queue->count + 1);
    }
    flt_queue_enqueue(s->queue, val);
}

static inline float data_stream_dequeue(struct data_stream *s)
{
    if (flt_queue_is_empty(s->queue)) {
        return FP_NAN;
    }
    s->avg = (s->avg * (float)(s->queue->count) - flt_queue_peek(s->queue)) / (float)(s->queue->count - 1);
    return flt_queue_dequeue(s->queue);
}

static inline void data_stream_print(struct data_stream *s)
{
    printf("stream: [");
    if (!flt_queue_is_empty(s->queue)) {
        printf(" %.2f", flt_queue_at(s->queue, 0));
    }
    for (size_t i = 1; i < s->queue->count; i++) {
        printf(", %.2f", flt_queue_at(s->queue, (uint32_t)i));
    }
    printf(" ]\n");
}

static inline bool float_is_nearly_equal(const float a, const float b, const uint16_t n)
{
    return fabsf(a - b) <= FLT_EPSILON * (float)n;
}

#define print_line(expr)        \
    __extension__({             \
        (expr);                 \
        printf("%-34s", #expr); \
    })

int main(void)
{
    struct data_stream s;
    data_stream_init(&s, 8);

    print_line(data_stream_enqueue(&s, 3.f));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (3.f) / (float)(1), 128));

    print_line(data_stream_enqueue(&s, 5.f));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (3.f + 5.f) / (float)(2), 128));

    print_line(data_stream_enqueue(&s, 15.f));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (3.f + 5.f + 15.f) / (float)(3), 128));

    print_line(data_stream_dequeue(&s));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (5.f + 15.f) / (float)(2), 128));

    print_line(data_stream_dequeue(&s));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (15.f) / (float)(1), 128));

    print_line(data_stream_enqueue(&s, 45.f));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (15.f + 45.f) / (float)(2), 128));

    print_line(data_stream_dequeue(&s));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(float_is_nearly_equal(s.avg, (45.f) / (float)(1), 128));

    print_line(data_stream_dequeue(&s));
    printf("average: %-10.2f ", s.avg);
    data_stream_print(&s);
    assert(isnanf(s.avg));

    data_stream_deinit(&s);
}

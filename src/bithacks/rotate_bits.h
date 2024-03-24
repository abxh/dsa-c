#include <assert.h> // assert
#include <limits.h> // CHAR_BIT
#include <stdlib.h> // size_t

static_assert(CHAR_BIT == 8, "a char is 8 bits");

// references:
// https://stackoverflow.com/questions/10134805/bitwise-rotate-left-function
// https://en.wikipedia.org/wiki/Circular_shift

static inline size_t rotate_bits_left(const size_t value, size_t count) {
    assert(count != 0);
    assert(count < sizeof(value) * 8);
    return (value << count) | (value >> (sizeof(value) * 8 - count));
}

static inline size_t rotate_bits_right(const size_t value, size_t count) {
    assert(count != 0);
    assert(count < sizeof(value) * 8);
    return (value << (sizeof(value) * 8 - count)) | value >> count;
}

static inline size_t rotate_bits(const size_t value, int shift) {
    assert(shift < (int)sizeof(value) * 8);
    assert(shift > -(int)sizeof(value) * 8);
    if (shift == 0 || shift == sizeof(size_t) * 8 || shift == -(int)sizeof(size_t) * 8) {
        return value;
    } else if (shift < 0) {
        return rotate_bits_left(value, -shift);
    } else {
        return rotate_bits_right(value, shift);
    }
}

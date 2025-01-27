/*
    The test below previously tested a function named `align_forward`.

    `align_forward` is replaced by `align`, but a translation of `align` to
    `align_forward` is tested instead here.

    Why? I am lazy.

    Pointer offsets (P):
    P := 0
    ...
    P := 128

    Alignments (A):
    A := 1
    A := 2
    A := 4
    A := 8

    align_forward returns either P, if (P % A) is 0,
    or P + A - (P % A) otherwise.
*/

#include "align.h"

#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

static inline uintptr_t align_forward(size_t p, size_t a)
{
    void *ptr = (void *)p;
    const size_t size = 0;
    size_t space = SIZE_MAX;
    return (uintptr_t)align(a, size, &ptr, &space);
}

int main(void)
{
    size_t *P = calloc(129, sizeof(uintptr_t));
    for (size_t i = 0; i < 129; i++) {
        P[i] = i;
    }

    size_t A[] = {1, 2, 4, 8};

    for (size_t j = 0; j < sizeof(A) / sizeof(*A); j++) {
        for (size_t i = 0; i < 129; i++) {
            if (P[i] % A[j] == 0) {
                assert(align_forward(P[i], A[j]) == P[i]);
                assert(calc_alignment_padding(A[j], P[i]) + P[i] == P[i]);
            }
            else {
                assert(align_forward(P[i], A[j]) == P[i] + A[j] - (P[i] % A[j]));
                assert(calc_alignment_padding(A[j], P[i]) + P[i] == P[i] + A[j] - (P[i] % A[j]));
            }
        }
    }

    free(P);
}

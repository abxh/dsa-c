/*
    Test cases (N):
    - N := 1
    - N := 2
    - N := 127
    - N := 128
    - N := 129
    - N := 768
    - N := 1e+6
    - N := 1e+9
    - N := UINT32_MAX / 2 + 1
*/

#include "round_up_pow2_32.h"
#include "math.h"

int main(void)
{
    {
        assert(round_up_pow2_32(1) == 1);
        assert(round_up_pow2_32_fallback(1) == 1);
        assert(round_up_pow2_32(2) == 2);
        assert(round_up_pow2_32_fallback(2) == 2);
        assert(round_up_pow2_32(127) == 128);
        assert(round_up_pow2_32_fallback(127) == 128);
        assert(round_up_pow2_32(128) == 128);
        assert(round_up_pow2_32_fallback(128) == 128);
        assert(round_up_pow2_32(129) == 256);
        assert(round_up_pow2_32_fallback(129) == 256);
        assert(round_up_pow2_32(768) == 1024);
        assert(round_up_pow2_32_fallback(768) == 1024);
        assert(round_up_pow2_32(1e+6) == (uint32_t)pow(2, round(log2(1e+6))));
        assert(round_up_pow2_32_fallback(1e+6) == (uint32_t)pow(2, round(log2(1e+6))));
        assert(round_up_pow2_32(1e+9) == (uint32_t)pow(2, round(log2(1e+9))));
        assert(round_up_pow2_32_fallback(1e+9) == (uint32_t)pow(2, round(log2(1e+9))));
        assert(round_up_pow2_32(UINT32_MAX / 2 + 1) == (uint32_t)pow(2, 31));
        assert(round_up_pow2_32_fallback(UINT32_MAX / 2 + 1) == (uint32_t)pow(2, 31));
    }
}

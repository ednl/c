#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <math.h>    // cbrtl, floorl
#include <assert.h>

// Cube root of 6 (long double where LDBL_DECIMAL_DIG = 21)
#define CR6 1.817120592832139658909L

// Largest tetrahedral number to fit in uint64_t is Te(4801278)
// Te(4801278)=18446741848412178987 < 2^64 < Te(4801279)=18446753374556197973
#define TETRACOUNT 4801279

// All tetrahedral numbers that fit in 64-bit
static uint64_t tetrahedral[TETRACOUNT];

static int tetralast(const uint64_t t)
{
    const int n = (int)(floorl(cbrtl(t) * CR6));  // 0 <= n <= 4801279
    return n < TETRACOUNT ? (tetrahedral[n] > t ? n - 1 : n) : TETRACOUNT - 1;
}

int main(void)
{
    tetrahedral[0] = 0;
    tetrahedral[1] = 1;
    for (int n = 2; n < TETRACOUNT; ++n)
        tetrahedral[n] = tetrahedral[n - 1] - tetrahedral[n - 2] + tetrahedral[n - 1] + (uint64_t)n;

    const uint64_t first = tetrahedral[2953] - 10ull;
    const uint64_t count = 20ull;
    const uint64_t last = first + count >= first ? first + count : UINT64_MAX;

    for (uint64_t t = first; t >= first && t <= last; ++t) {
        const int n = tetralast(t);

        const uint64_t last = tetrahedral[n];
        const uint64_t next = last+1 < TETRACOUNT ? tetrahedral[last+1] : 0;
        printf("%llu: Te(%d)=%llu\n", t, n, last);

        assert(last <= t);
        assert(t < next || !next);
    }
    return 0;
}

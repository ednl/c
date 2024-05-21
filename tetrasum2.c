#include <stdio.h>
#include <stdlib.h>    // malloc, free
#include <math.h>      // cbrt, floor
#include <stdint.h>    // uint32_t, UINT32_MAX
#include <inttypes.h>  // PRIu32
#include <stdbool.h>

static uint32_t *tetra;

static bool solution(uint32_t i, uint32_t j, uint32_t k, uint32_t l, uint32_t m)
{
    bool dup = false;
    printf("%"PRIu32, tetra[i]);
    if (j) { printf(" + %"PRIu32, tetra[j]); dup = dup || i == j; }
    if (k) { printf(" + %"PRIu32, tetra[k]); dup = dup || j == k; }
    if (l) { printf(" + %"PRIu32, tetra[l]); dup = dup || k == l; }
    if (m) { printf(" + %"PRIu32, tetra[m]); dup = dup || l == m; }
    printf("\n");
    return dup;
}

int main(int argc, char *argv[])
{
    uint32_t target = 343867;
    if (argc > 1) {
        const long long a = atoll(argv[1]);
        if (a > 0 && a <= UINT32_MAX)
            target = (uint32_t)a;
    }

    const uint32_t N = 1 + (uint32_t)(floor(cbrt(target) * cbrt(6)));
    tetra = malloc(N * sizeof *tetra);
    for (uint32_t n = 0; n < N; ++n)
        tetra[n] = ((n * (n + 1)) >> 1) * (n + 2) / 3;

    uint32_t total = 0, dup = 0, part[5] = {0};
    for (uint32_t i = 1; i < N; ++i) {
        uint32_t a = tetra[i];
        if (a >= target) { if (a == target) { ++total; ++part[0]; dup += solution(i, 0, 0, 0, 0); } break; }
        for (uint32_t j = i; j < N; ++j) {
            uint32_t b = a + tetra[j];
            if (b >= target) { if (b == target) { ++total; ++part[1]; dup += solution(i, j, 0, 0, 0); } break; }
            for (uint32_t k = j; k < N; ++k) {
                uint32_t c = b + tetra[k];
                if (c >= target) { if (c == target) { ++total; ++part[2]; dup += solution(i, j, k, 0, 0); } break; }
                for (uint32_t l = k; l < N; ++l) {
                    uint32_t d = c + tetra[l];
                    if (d >= target) { if (d == target) { ++total; ++part[3]; dup += solution(i, j, k, l, 0); } break; }
                    // Try to find fifth term:
                    uint32_t res = target - d;  // residue
                    if (res < tetra[l] || res > tetra[N - 1]) continue;
                    if (res == tetra[l]) { ++total; ++part[4]; dup += solution(i, j, k, l, l); continue; }
                    if (res == tetra[N - 1]) { ++total; ++part[4]; dup += solution(i, j, k, l, N - 1); continue; }
                    uint32_t min = l, max = N - 1, m;
                    while (min < max) {
                        m = ((min + 1) >> 1) + (max >> 1);
                        if (m == min || m == max) break;
                        if      (tetra[m] < res) min = m;
                        else if (tetra[m] > res) max = m;
                        else { ++total; ++part[4]; dup += solution(i, j, k, l, m); break; };
                    }
                }
            }
        }
    }

    if (total)
        printf("\n");
    printf("Target          : %"PRIu32"\n", target);
    printf("Sums found      : %"PRIu32"\n", total);
    printf("All distinct    : %"PRIu32"\n", total - dup);
    printf("With duplicates : %"PRIu32"\n", dup);
    printf("Terms 1,2,3,4,5 : %"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"\n", part[0], part[1], part[2], part[3], part[4]);

    free(tetra);
    return 0;
}

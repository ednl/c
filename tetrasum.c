#include <stdio.h>
#include <stdlib.h>  // atoll
#include <stdint.h>  // UINT32_MAX

// The N'th tetrahedral number is greater than the sum
#define N 128

// Tetrahedral numbers 0..N-1
static unsigned tetra[N];

// Search for a sum of 'take' distinct tetrahedral numbers
static unsigned take = 5;
static unsigned sum = 343867;

// Calculate the n'th tetrahedral number
static unsigned tetracalc(const unsigned n)
{
    return ((n * (n + 1)) >> 1) * (n + 2) / 3;
}

int main(int argc, char *argv[])
{
    if (argc > 3) {
        fprintf(stderr, "Use 0, 1 or 2 arguments.\n");
        return 1;
    }
    if (argc > 1) {
        long long t = atoll(argv[1]);
        if (t > 0 && t <= UINT32_MAX)
            sum = (unsigned)t;
        else {
            fprintf(stderr, "Argument 1 out of range.\n");
            return 2;
        }
    }
    if (argc == 3) {
        long long t = atoll(argv[2]);
        if (t > 0 && t < 128)
            take = (unsigned)t;
        else {
            fprintf(stderr, "Argument 2 out of range.\n");
            return 3;
        }
    }
    printf("Find %u tetrahedral numbers that sum to %u\n", take, sum);
    printf("------------------------------------------------\n");

    // Precompute the first N tetrahedral numbers
    for (unsigned i = 0; i < N; ++i)
        tetra[i] = tetracalc(i);

    // TODO: use combinations algorithmn to enumerate all unique k-subsets (where k=take)
    // of the n-set (where n=N) of tetrahedral numbers, e.g. with Gray code.
    unsigned solutions = 0;
    for (unsigned i0 = 0; i0 < N - 4; ++i0) {
        unsigned p0 = tetra[i0];
        if (p0 >= sum) break;
        for (unsigned i1 = i0 + 1; i1 < N - 3; ++i1) {
            unsigned p1 = p0 + tetra[i1];
            if (p1 >= sum) break;
            for (unsigned i2 = i1 + 1; i2 < N - 2; ++i2) {
                unsigned p2 = p1 + tetra[i2];
                if (p2 >= sum) break;
                for (unsigned i3 = i2 + 1; i3 < N - 1; ++i3) {
                    unsigned p3 = p2 + tetra[i3];
                    if (p3 >= sum) break;
                    for (unsigned i4 = i3 + 1; i4 < N; ++i4) {
                        unsigned p4 = p3 + tetra[i4];
                        if (p4 > sum) break;
                        if (p4 == sum)
                            printf("%u: T(%u)+T(%u)+T(%u)+T(%u)+T(%u) = %u+%u+%u+%u+%u = %u\n",
                                ++solutions, i0, i1, i2, i3, i4, tetra[i0], tetra[i1], tetra[i2], tetra[i3], tetra[i4], sum);
                    }
                }
            }
        }
    }
    return 0;
}

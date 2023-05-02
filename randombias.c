#include <stdio.h>
#include <stdlib.h>      // rand, drand48, random, arc4random (macOS)
#include <stdint.h>      // uint64_t
#include <inttypes.h>    // PRIu64
#include <string.h>      // memset
#include <math.h>        // trunc
#include <time.h>        // time for srand48/srand/srandom seed
#ifdef __linux__
#include <bsd/stdlib.h>  // arc4random (apt install libbsd-dev, link option: -lbsd)
#endif
#include "startstoptimer.h"  // compile with extra source file: startstoptimer.c

// Number of die faces
#define N 6

// Values on die faces
static const uint64_t facevalue[N] = {1,2,3,4,5,6};

// Die-roll function signature
typedef unsigned int (*func_t)(void);

// Unbiased die-roll using drand48()
static unsigned int roll_drand48(void)
{
    return (unsigned int)(trunc(drand48() * N));
}

// Unbiased die-roll using rand()
// Ref.: https://en.cppreference.com/w/c/numeric/random/rand
// In modern implementations of stdlib.c, rand() is probably the same as random()
static unsigned int roll_rand(void)
{
    unsigned int x = N;
    while (x == N)
        x = (unsigned int)rand() / ((RAND_MAX + 1U) / N);
    return x;
}

// Unbiased die-roll using random()
static unsigned int roll_random(void)
{
    unsigned long x = N;
    while (x == N)
        x = (unsigned long)random() / ((RAND_MAX + 1UL) / N);
    return (unsigned int)x;
}

// Unbiased die-roll using arc4random()
static unsigned int roll_arc4random(void)
{
    return (unsigned int)arc4random_uniform(N);
}

int main(void)
{
    const char * const funcname[] = {"drand48", "rand", "random", "arc4random"};
    const func_t func[] = {roll_drand48, roll_rand, roll_random, roll_arc4random};
    const size_t funcs = sizeof func / sizeof *func;
    const char * const mult[] = {"", "thousand", "million", "billion"};
    const size_t mults = sizeof mult / sizeof *mult;
    const size_t multmax = mults - 1;  // maximum index into mult[]
    const uint64_t dots = N < 10 ? 10 * N : N;  // number of progress dots

    // Theoretical population mean (or: sample mean for samples->infinity)
    uint64_t facesum = 0;
    for (int i = 0; i < N; ++i)
        facesum += facevalue[i];
    long double popmean = (long double)facesum / N;

    // Theoretical population variance
    long double popvar = 0;
    for (int j = 0; j < N; ++j) {
        long double d = (long double)facevalue[j] - popmean;
        popvar += d * d;
    }
    popvar /= N;

    uint64_t samples = N;  // number of rolls per RNG
    uint64_t hist[N];  // histogram, reset before use

    // Title
    printf("-------------------------------------------------------------------------------------------------------\n");
    printf("Check bias of different random number generators available in the C standard library\n");
    printf("-------------------------------------------------------------------------------------------------------\n");
    printf("Face values = %"PRIu64, facevalue[0]);
    for (int i = 1; i < N; ++i)
        printf(",%"PRIu64, facevalue[i]);
    printf(" (%u)\n", N);
    printf("Mean        = %.8Lf\n", popmean);
    printf("Variance    = %.8Lf\n\n", popvar);

    // Seed RNGs
    srand48((long)time(NULL));
#ifdef __APPLE__
    sranddev();
    srandomdev();
#else
    srand((unsigned int)time(NULL));
    srandom((unsigned int)time(NULL));
#endif

    // Break out of loop after 'samples' gets to billions
    while (1) {
        // Progress bar parameters
        uint64_t intervals, length;
        if (samples <= dots) {
            intervals = 1;
            length = samples;
        } else if (!(samples % dots)) {
            intervals = dots;
            length = samples / dots;
        } else {
            fprintf(stderr, "Internal error: %"PRIu64"/%"PRIu64" has remainder.\n", samples, dots);
            exit(1);
        }

        // Header
        uint64_t n = samples, m = 0;
        while (n > 1000 && m < multmax) {
            n /= 1000;
            m++;
        }
        printf("Rolls per RNG = %3"PRIu64" %-8s", n, mult[m]);
        n = dots - 12;
        while (n--)
            printf(" ");
        printf("meandev      vardev    time\n");

        // Test every RNG
        for (unsigned int i = 0; i < funcs; ++i) {
            printf("%-11s", funcname[i]);
            n = dots - intervals;  // alignment when intervals < dots
            while (n--)
                printf(".");

            const func_t f = func[i];  // current RNG
            memset(hist, 0, sizeof hist);  // clear histogram
            starttimer();
            for (uint64_t j = 0; j < intervals; ++j) {
                printf(".");  // progress bar
                fflush(stdout);
                for (uint64_t k = 0; k < length; ++k)
                    hist[f()]++;
            }
            double t = stoptimer_s();

            // Sample mean, where sum(hist) = samples
            uint64_t prodsum = 0;
            for (int j = 0; j < N; ++j)
                prodsum += hist[j] * facevalue[j];
            long double mean = (long double)prodsum / samples;

            // Sample variance, where sum(hist) = samples
            long double variance = 0;
            for (int j = 0; j < N; ++j) {
                long double d = (long double)facevalue[j] - mean;
                variance += d * d * hist[j];
            }
            variance /= samples;

            printf("%+12.8Lf%+12.8Lf%8.3f\n", mean - popmean, variance - popvar, t);
        }

        // Enough is enough
        if (m == multmax)
            break;

        printf("\n");
        samples *= 10;
    }
    return 0;
}

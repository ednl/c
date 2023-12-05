#include <stdio.h>    // printf
#include <stdlib.h>   // drand48
#include <math.h>     // pow, sin, sqrt
#include <time.h>     // time (for srand48)
#include <stdbool.h>  // bool
#include <stdint.h>   // uint64_t

typedef struct Vec {
    double x[2];
} Vec;

typedef struct Span {
    Vec vec[2];
} Span;

static double len(const Vec v)
{
    return v.x[1] - v.x[0];
}

static double randseg(const Vec v)
{
    return v.x[0] + drand48() * len(v);
}

static double f(const double x)
{
    return pow(x, x) * sin(x);
}

static double g(const double x)
{
    return sin(x) * 4;
}

int main(void)
{
    // Area of interest
    const Span span = {{{2, M_PI}, {0, 7}}};
    const double rect = len(span.vec[0]) * len(span.vec[1]);
    const double maxerr = 0.005;

    // Init
    srand48(time(NULL));
    const uint64_t chunk = 10000;
    uint64_t hit = 0, count = 0;
    double mean = 0, M2 = 0, std;

    // Iterate until error small enough
    do {
        for (uint64_t i = 0; i < chunk; ++i) {
            ++count;
            // Probe point inside square
            const double x = randseg(span.vec[0]);
            const double y = randseg(span.vec[1]);
            // Evaluate hit or miss
            hit += y <= f(x) && y >= g(x);
            // Currently estimated value
            const double val = ((double)hit / count) * rect;
            // Update running mean and unscaled variance
            const double delta = val - mean;
            mean += delta / count;
            M2 += delta * (val - mean);
        }
        // Scale variance & take square root for standard deviation
        std = sqrt(M2 / count);
    } while (std > maxerr);

    // Result
    printf("N=%llu: area = %.3f +/- %.3f\n", count, mean, std);
    return 0;
}

#include <stdio.h>    // printf
#include <stdlib.h>   // drand48
#include <math.h>     // sqrt
#include <time.h>     // time (for srand48)

// Radius and diameter of all three cylinders of the Steinmetz solid
#define R 1.0
#define D (R * 2)

// Simulation parameters
#define ERROR 0.005
#define BATCH 10000

// Monte Carlo volume: cube tightly enclosing tricylinder
#define CUBE (D * D * D)

// Volume of tricylinder with r=1: 8(2-sqrt(2))
#define CALC ((2 - M_SQRT2) * CUBE)

static double rnd(void)
{
    return drand48() * 2 - 1;
}

static inline double sqr(const double x)
{
    return x * x;
}

int main(void)
{
    // Init
    srand48(time(NULL));
    unsigned hit = 0, count = 0;
    double mean = 0, M2 = 0, stddev;

    // Iterate until error small enough
    do {
        for (int i = 0; i < BATCH; ++i) {
            ++count;
            // Probe point inside unit cube, squared
            const double x = sqr(rnd());
            const double y = sqr(rnd());
            const double z = sqr(rnd());
            // Evaluate hit or miss
            hit += x + y <= 1 && x + z <= 1 && y + z <= 1;
            // Currently estimated value
            const double est = ((double)hit / count) * CUBE;
            // Update running mean and unscaled variance
            const double delta = est - mean;
            mean += delta / count;
            M2 += delta * (est - mean);
        }
        // Scale variance & take square root for standard deviation
        stddev = sqrt(M2 / count);
    } while (stddev > ERROR);

    // Result
    printf("calculation =  %.5f\n", CALC);
    printf("monte carlo =  %.3f ± %.3f (N=%u)\n", mean, stddev, count);
    printf("      error = %+.4f\n", mean - CALC);
    return 0;
}

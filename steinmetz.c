#include <stdio.h>    // printf
#include <stdlib.h>   // drand48
#include <math.h>     // sqrt
#include <time.h>     // time (for srand48)
#include <stdbool.h>  // bool

// Radius and diameter of all three cylinders of the Steinmetz solid
#define R 1.0
#define D (R * 2)

// Simulation parameters
#define ERROR 0.005
#define BATCH 10000

// Monte Carlo reference volume: cube that encloses tricylinder
#define CUBE (D * D * D)

// Volume of tricylinder for r=1 is (2-sqrt(2)).(2r)^3
#define CALC ((2 - M_SQRT2) * CUBE)

// Random coordinate inside double unit cube around origin
static double rnd(void)
{
    return drand48() * 2 - 1;
}

// Convenience function for squaring
static inline double sqr(const double x)
{
    return x * x;
}

// Evaluate next Monte Carlo sample: inside (true) or out (false)
static bool sample(void)
{
    // Probe point inside double unit cube, squared
    const double a = sqr(rnd());
    const double b = sqr(rnd());
    const double c = sqr(rnd());
    // Evaluate hit or miss
    return a + b <= R && a + c <= R && b + c <= R;
}

int main(void)
{
    // Init
    srand48(time(NULL));
    unsigned hit = 0, count = 0;      // Monte Carlo simulation counters
    double mean = 0, M2 = 0, stddev;  // running mean, unscaled variance, standard deviation

    // Monte Carlo method until standard deviation is small enough
    do {
        // Single iteration wouldn't change stddev much
        for (int i = 0; i < BATCH; ++i) {
            // Get new Monte Carlo sample
            ++count;
            hit += sample();
            // Currently estimated value
            const double frac = (double)hit / count;
            const double estm = frac * CUBE;
            // Update running mean and unscaled variance
            // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
            const double delta = estm - mean;
            mean += delta / count;
            M2 += delta * (estm - mean);
        }
        // Scale as population variance (= exact variance of given data)
        // and take square root for new standard deviation
        stddev = sqrt(M2 / count);
    } while (stddev > ERROR);

    // Results
    printf("calculation =  %.5f\n", CALC);
    printf("monte carlo =  %.3f ± %.3f (N=%u)\n", mean, stddev, count);
    printf("      error = %+.4f\n", mean - CALC);
    return 0;
}

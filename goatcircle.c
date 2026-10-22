/**
 * Programmeeropdracht voor eerstejaars natuurkunde aan de UvA 2026
 * via Ivo van Vulpen https://mu.social/profile/ivovanvulpen.bsky.social/post/3myfxhxx4ks2l
 *
 * Problem:
 *   Grass circle has centre (0,0) and radius R, goat circle has centre (R,0)
 *   and radius 0 < r < 2R. Find smallest r accurate to two decimals where
 *   intersection of grass and goat circles is at least 50% of grass area.
 *   Use Monte Carlo simulation to determine area of intersection.
 * Given:
 *   Grass circle: x^2 + y^2 = R^2
 *   Goat circle: (x - R)^2 + y^2 = r^2
 *     => x^2 - 2Rx + R^2 + y^2 = r^2
 *     => x^2 + y^2 = r^2 - R^2 + 2Rx
 *
 * Compile:
 *   cc -std=c17 -Wall -Wextra -pedantic -O3 -march=native -mtune=native goatcircle.c
 * Run:
 *   ./a.out
 * Output for R=1, PCT=50:
 *   Goat eats 50% of grass at r=1.16 +/- 0.005
 */

#include <stdio.h>
#include <time.h>    // time
#include <stdlib.h>  // srand48, drand48
#include <stdint.h>  // uint64_t
#include <stdbool.h>

// Problem parameters
#define R    1      // grass radius
#define PCT  50     // target percentage
#define ERR  0.005  // "two decimals"

// Values used for solution
#define PI   3.14159265359          // pi (also available as M_PI in math.h)
#define R2   (R * R)                // grass radius squared
#define BASE (R2 * 4)               // probe area = (2R)^2
#define GOAL (PI * R2 * PCT / 100)  // percentage of circle πR^2
#define RMIN 0                      // goat radius minimum
#define RMAX (R * 2)                // goat radius maximum
#define ERR2 (ERR * ERR)            // error squared

// Standard deviation = sqrt(M2/count)
typedef struct runningmean {
    uint64_t hit, count;
    double mean, M2;
} RunningMean;

// Random real value from [-R,+R)
static double probe(void)
{
    return drand48() * (R * 2) - R;
}

// Test if probe is inside intersection of circles
// parameter: r^2 - R^2
static bool mc_test(const double r2_R2)
{
    const double x = probe();
    const double y = probe();
    const double L2 = x * x + y * y;  // L2 norm (squared)
    return L2 < R2 && L2 < x * (R * 2) + r2_R2;
}

// Monte Carlo simulation of 100 points
// = initial guess of area for radius r
static double mc_init(const double r)
{
    const double r2_R2 = r * r - R2;
    unsigned hit = 0;
    for (int i = 0; i < 100; ++i)
        hit += mc_test(r2_R2);
    return (double)hit / 100 * BASE;
}

// Monte Carlo simulation of 'count' points
//   with update of running mean and (unscaled) variance
// Return: updated standard deviation (squared)
static double mc_chunk(RunningMean *rm, const uint64_t count, const double r)
{
    const double r2_R2 = r * r - R2;
    for (uint64_t i = 0; i < count; ++i) {
        // Evaluate hit or miss of probe (x,y)
        rm->hit += mc_test(r2_R2);
        // Currently estimated value
        rm->count++;
        const double val = (double)rm->hit / rm->count * BASE;
        // Update running mean and (unscaled) variance
        const double delta = val - rm->mean;
        rm->mean += delta / rm->count;
        rm->M2 += delta * (val - rm->mean);
    }
    // Scale variance to get standard deviation (squared)
    return rm->M2 / rm->count;
}

// Monte Carlo simulation until area is known
//   with certainty defined as max value for standard deviation
static double area(const double r)
{
    RunningMean rm = {.hit=0, .count=0, .mean=mc_init(r), .M2=0};
    while (mc_chunk(&rm, 10000, r) > ERR2)
        ;
    // DEBUG: print total count needed to make stddev<ERR
    //   For R=1,PCT=50,ERR=0.005 with initialised mean:
    //   count is between about 500,000 and 6,000,000
    // printf("%8llu\n", rm.count);
    return rm.mean;
}

int main(void)
{
    // Seed random number generator
    srand48(time(NULL));

    // Binary search for goal under assumption that:
    // min < max, area(min) < GOAL, area(max) > GOAL
    //   For R=1,PCT=50,ERR=0.005 with initialised mean: takes 9 loops
    double min = RMIN, max = RMAX, r = 0;
    while (max - min > ERR) {
        r = (min + max) / 2;  // average
        if (area(r) <= GOAL) min = r;
        else                 max = r;
    }

    // Solution
    // R=1,PCT=50,ERR=0.005: r=1.16
    printf("Goat eats %u%% of grass at r=%.2f +/- %.3f\n", PCT, r, ERR);
}

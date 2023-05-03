#include <stdio.h>     // printf, fprintf, fputc
#include <stdlib.h>    // rand, drand48, random, arc4random (macOS), exit, malloc, calloc, free
#include <stdint.h>    // uint64_t, int64_t
#include <inttypes.h>  // PRIu64, PRId64
#include <math.h>      // trunc
#include <time.h>      // time for srand48/srand/srandom seed
#include <signal.h>    // catch ^C
#ifdef __linux__
#include <bsd/stdlib.h>  // arc4random (apt install libbsd-dev, link option: -lbsd)
#endif
#include "startstoptimer.h"  // compile with extra source file: startstoptimer.c

// Allowed range for number of die faces
#define NMIN 2
#define NDEF 6
#define NMAX 1024

// Die-roll function signature
typedef unsigned int (*func_t)(void);

// Number of die faces, can be changed via command line arguments
//   One command line argument: custom number of faces
//   Two or more arguments: custom face values (count = new number of faces)
static unsigned int N = NDEF;

// Standard or custom face values, histogram per RNG
// (dynamically allocated, free in interrupt handler or at end of program)
static  int64_t *facevalue = NULL;
static uint64_t *hist = NULL;

// Interrupt handler for SIGINT (^C)
static void intHandler(int sig)
{
    if (sig == SIGINT) {
        fputc('\n', stdout);
        free(facevalue);
        free(hist);
        exit(0);
    }
}

// Die-roll [0..N-1] using drand48()
static unsigned int roll_drand48(void)
{
    return (unsigned int)(trunc(drand48() * N));  // extra parentheses to avoid cast warning
}

// Unbiased die-roll [0..N-1] using rand()
// Ref.: https://en.cppreference.com/w/c/numeric/random/rand
// In modern implementations of stdlib.c, rand() is probably the same type of RNG as random()
static unsigned int roll_rand(void)
{
    unsigned int x = N;
    while (x == N)
        x = (unsigned int)rand() / ((RAND_MAX + 1U) / N);
    return x;
}

// Unbiased die-roll [0..N-1] using random()
// Ref.: https://en.cppreference.com/w/c/numeric/random/rand
static unsigned int roll_random(void)
{
    unsigned long x = N;
    while (x == N)
        x = (unsigned long)random() / ((RAND_MAX + 1UL) / N);
    return (unsigned int)x;
}

// Die-roll [0..N-1] using arc4random()
static unsigned int roll_arc4random(void)
{
    return arc4random_uniform(N);
}

// Snap to range (0 <= r0 <= r1)
static unsigned int snap(const int r0, const int r1, const int a)
{
    return (unsigned int)(a < r0 ? r0 : (a > r1 ? r1 : a));
}

int main(int argc, char * argv[])
{
    const char * const rngname[] = {"drand48", "rand", "random", "arc4random"};
    const func_t rngfunc[] = {roll_drand48, roll_rand, roll_random, roll_arc4random};
    const size_t rngcount = sizeof rngfunc / sizeof *rngfunc;
    const char * const multname[] = {"", "thousand", "million", "billion"};
    const size_t multsize = sizeof multname / sizeof *multname;
    const size_t multmax = multsize - 1;  // maximum index into multname[]
    const uint64_t sampleinc = 10;  // every next round, sample size increases by this factor
    double sampletime[rngcount] = {0};

    // Number of die faces
    if (argc < 2)
        N = NDEF;
    else if (argc == 2)
        N = snap(NMIN, NMAX, atoi(argv[1]));
    else
        N = (unsigned int)(argc - 1);

    // Catch ^C, to free allocated memory before exit
    struct sigaction ctrlc;
    ctrlc.sa_handler = intHandler;
    sigaction(SIGINT, &ctrlc, NULL);

    // Allocate memory for face values and histogram
    facevalue = malloc(N * sizeof *facevalue);  // may contain negative values via command line arguments
    hist = calloc(rngcount * N, sizeof *hist);  // init to zero
    if (!facevalue || !hist) {
        fprintf(stderr, "Memory allocation failure.\n");
        free(facevalue);
        free(hist);
        exit(1);
    }

    // NB: ((sampleinc - 1) * samples) should be divisible by 'dots'
    const uint64_t dots = N * 9;  // number of progress dots
    uint64_t prevsamples = 0, samples = N;  // number of rolls per RNG

    // Set face values
    for (unsigned int i = 0; i < N; ++i)
        facevalue[i] = argc <= 2 ? i + 1 : atoi(argv[i + 1]);  // standard or custom die face values

    // Population mean (or: sample mean for sample size->infinity)
    int64_t facesum = 0;
    for (unsigned int i = 0; i < N; ++i)
        facesum += facevalue[i];
    const long double popmean = (long double)facesum / N;

    // Population variance
    long double popvar = 0;
    for (unsigned int i = 0; i < N; ++i) {
        long double d = (long double)facevalue[i] - popmean;
        popvar += d * d;
    }
    popvar /= N;

    // Title
    printf("---------------------------------------------------------------------------------------------------------\n");
    printf("Check bias of different random number generators in the C standard library on macOS and Linux\n");
    printf("---------------------------------------------------------------------------------------------------------\n");
    printf("Face values         = [%"PRId64, facevalue[0]);
    for (unsigned int i = 1; i < N; ++i)
        printf(",%"PRId64, facevalue[i]);
    printf("]\nPopulation mean     = %.8Lf\n", popmean);
    printf("Population variance = %.8Lf\n\n", popvar);
    printf("For the whole population (or: infinite sample size), all face values should occur at the same frequency.\n");
    printf("In other words, the ideal distribution variance and thus standard deviation is zero.\n");
    printf("Sample sizes are cumulative! The roll tally (histogram, or frequency distribution) is not reset\n");
    printf("between increases in sample size. So all characteristics are progressive (per RNG).\n\n");
    printf("Characteristics listed per sample size per RNG:\n");
    printf("* meandiff = difference from the population mean\n");
    printf("* vardiff  = difference from the population variance\n");
    printf("* stddev   = frequency distribution standard deviation, scaled by sample size\n");
    printf("* time     = measured time in seconds (single thread)\n\n");

    // Seed RNGs
    time_t tt = time(NULL);
    srand48((long)tt);
#ifdef __APPLE__
    sranddev();
    srandomdev();
#else
    srand((int)tt);
    srandom((int)tt);
#endif

    // Break out of loop after sample size gets to billions
    while (1) {
        // Progress bar parameters
        const uint64_t addsamples = samples - prevsamples;  // how many more samples to add this loop
        uint64_t intervals, length;
        if (addsamples <= dots) {
            intervals = 1;
            length = addsamples;
        } else if (!(addsamples % dots)) {
            intervals = dots;
            length = addsamples / dots;
        } else {
            fprintf(stderr, "Internal error: %"PRIu64"/%"PRIu64" has remainder.\n", addsamples, dots);
            free(facevalue);
            free(hist);
            exit(2);
        }

        // Header
        uint64_t n = samples, order = 0;
        while (n > 1000 && order < multmax) {
            n /= 1000;
            order++;
        }
        printf("Rolls per RNG = %3"PRIu64" %-8s", n, multname[order]);
        n = dots - 13;
        while (n--)
            printf(" ");
        printf("meandiff     vardiff  stddev    time\n");

        // Test every RNG
        for (size_t i = 0; i < rngcount; ++i) {
            printf("%-11s", rngname[i]);
            const func_t f = rngfunc[i];  // current RNG function
            uint64_t * const curhist = &hist[i * N];  // curhist[j] = hist[i][j] (i<rngcount,j<N)

            // New set of rolls for this RNG
            starttimer();
            for (uint64_t j = 0; j < intervals; ++j) {
                printf(".");  // progress bar
                fflush(stdout);
                for (uint64_t k = 0; k < length; ++k)
                    curhist[f()]++;
            }
            sampletime[i] += stoptimer_s();

            // Fix alignment when intervals < dots
            n = dots - intervals;
            while (n--)
                printf(".");

            // Sample mean, where sum(hist) = samples
            int64_t prodsum = 0;
            for (unsigned int j = 0; j < N; ++j)
                prodsum += facevalue[j] * (int64_t)curhist[j];
            long double mean = (long double)prodsum / samples;

            // Sample variance, where sum(hist) = samples
            long double variance = 0;
            for (unsigned int j = 0; j < N; ++j) {
                long double d = (long double)facevalue[j] - mean;
                variance += d * d * curhist[j];
            }
            variance /= samples;

            // Frequency distribution standard deviation
            const long double meanfreq = (long double)samples / N;  // potentially fractional if program setup changes
            long double stddev = 0;
            for (unsigned int j = 0; j < N; ++j) {
                long double d = (long double)curhist[j] - meanfreq;
                stddev += d * d;  // perhaps scale here already to avoid overflow? not necessary for current setup
            }
            // Scaling by sample size is not strictly correct but this gives consistent values around 1
            // (N and not N-1 because this is the whole population of frequencies)
            stddev = sqrtl((stddev / N) / samples);

            // Show me the money
            printf("%+12.8Lf%+12.8Lf%8.5Lf%8.3f\n", mean - popmean, variance - popvar, stddev, sampletime[i]);
        }

        // Enough is enough
        if (order == multmax)
            break;

        printf("\n");
        prevsamples = samples;
        samples *= sampleinc;
    }

    free(facevalue);
    free(hist);
    return 0;
}

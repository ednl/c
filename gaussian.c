#include <stdio.h>
#include <stdlib.h>   // srandom, random, RAND_MAX, qsort, NULL
#include <time.h>     // time
#include <stdbool.h>  // bool, true, false
#include <math.h>     // sqrt, log, truncf

#define DEBUG 0       // set to non-zero to show data

#define KM_PER_MI 1.609344f
#define MI_PER_KM (1 / KM_PER_MI)

#define N      100    // number of days
#define MEAN   100.0  // average distance per day (km)
#define STDDEV  50.0  // standard deviation (km)
#define ALMOST   9    // threshold of attainable goal (km)

static float data[N];

#if DEBUG
static void show(const float *a, const int n)
{
    for (int i = 0; i < n; ++i) {
        if (i && !(i % 10))
            putc('\n', stdout);
        printf("%6.1f", a[i]);
    }
    putc('\n', stdout);
}
#endif

// Horizontal ruler
static void hr(void)
{
    puts("----------------------");
}

// Qsort helper: sort floats in descending order
static int float_desc(const void *p, const void *q)
{
    const float a = *(const float *)p;
    const float b = *(const float *)q;
    if (a < b) return +1;
    if (a > b) return -1;
    return 0;
}

// Marsaglia polar method for generating a pair of standard normal
// random values (= polar variant of the Box-Muller method)
// https://en.wikipedia.org/wiki/Marsaglia_polar_method
// NB: not thread-safe because of local static variables
static double gaussian(const double mean, const double stddev)
{
    static double spare = 0;
    static bool hasspare = false;
    if (hasspare) {
        hasspare = false;
        return spare * stddev + mean;
    }
    double u, v, s;
    do {
        u = ((double)random() / RAND_MAX) * 2.0 - 1.0;
        v = ((double)random() / RAND_MAX) * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0);
    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    hasspare = true;
    return u * s * stddev + mean;
}

static int eddington(float *a, const int n, const int more)
{
    if (!a || n <= 0)
        return -1;
    qsort(a, n, sizeof *a, float_desc);
#if DEBUG
    show(a, n);
#endif
    for (int i = 0, j; i < n; i = j) {
        const int x = truncf(a[i]);  // sample value is at least `x`
        // Skip same (truncated) values
        for (j = i + 1; j < n && truncf(a[j]) == x; ++j);
        // Eddington: value of at least `x` occurs `x` or more times
        if (x <= j) {
            printf("%3d : EDDINGTON\n", x);
            return x;  // stop at max Eddington
        }
        // Need a few more samples of (at least) `x` to make Eddington
        if (x - j <= more)
            printf("%3d : need %d more\n", x, x - j);
    }
    return 0;
}

int main(void)
{
    // generate standard normal float data >= 0
    srandom(time(NULL));
    for (int i = 0; i < N; ++i) {
        float sample = gaussian(MEAN, STDDEV);
        data[i] = sample >= 0 ? sample : 0;
    }

    // find max Eddington number in data as kilometres
    hr();
    printf("Kilometres (max +%d km)\n", ALMOST);
    hr();
    const int km = eddington(data, N, ALMOST);

    // convert to miles
    const int almost = roundf(ALMOST * MI_PER_KM);
    for (int i = 0; i < N; ++i)
        data[i] *= MI_PER_KM;
    hr();
    printf("Miles (max +%d mi)\n", almost);
    hr();
    const int mi = eddington(data, N, almost);

    hr();
    printf("km/mi = %.3f\n", KM_PER_MI);
    printf("%d/%d = %.3f\n", km, mi, (double)km / mi);

    return 0;
}

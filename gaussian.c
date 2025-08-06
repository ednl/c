#include <stdio.h>
#include <stdlib.h>   // srandom, random, RAND_MAX, qsort, NULL
#include <time.h>     // time
#include <stdbool.h>  // bool, true, false
#include <math.h>     // sqrt, log

#define N      100
#define MEAN   100.0
#define STDDEV  50.0
#define ALMOST  10

static float data[N];

// Qsort helper: sort floats in descending order.
static int float_desc(const void *p, const void *q)
{
    const float a = *(const float *)p;
    const float b = *(const float *)q;
    if (a < b) return +1;
    if (a > b) return -1;
    return 0;
}

// Marsaglia polar method for generating a pair of standard normal
// random values (= polar variant of the Box-Muller method).
// https://en.wikipedia.org/wiki/Marsaglia_polar_method
// NB: not thread-safe because of local static variables.
double gaussian(const double mean, const double stddev)
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

int main(void)
{
    // Generate standard normal float data >= 0
    srandom(time(NULL));
    for (int i = 0; i < N; ++i) {
        float sample = gaussian(MEAN, STDDEV);
        data[i] = sample >= 0 ? sample : 0;
    }

    // Find max Eddington number in data.
    qsort(data, N, sizeof *data, float_desc);
    for (int i = 0, j; i < N; ++i) {
        const int x = trunc(data[i]);  // sample value is at least `x`
        // Skip same (truncated) values
        for (j = i + 1; j < N && trunc(data[j]) == x; ++j);
        // Eddington: value of at least `x` occurs `x` or more times
        if (x <= j) {
            printf("%3d : EDDINGTON\n", x);
            break;  // stop at max Eddington because sorted descending
        }
        // Need a few more samples of (at least) `x` to make Eddington
        if (x - j < ALMOST)
            printf("%3d : need %d more\n", x, x - j);
    }
}

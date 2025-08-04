#include <stdio.h>
#include <stdlib.h>  // random, srandom, RAND_MAX, qsort
#include <math.h>    // sqrt, log
#include <time.h>    // time
#include <stdbool.h>

#define N 1024
static float dist[N];

static int float_desc(const void *p, const void *q)
{
    const float a = *(const float *)p;
    const float b = *(const float *)q;
    if (a < b) return +1;
    if (a > b) return -1;
    return 0;
}

double gaussian(double mean, double stddev)
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
    srandom(time(NULL));
    for (int i = 0; i < N; ++i) {
        float km = gaussian(100, 50);
        if (km < 0)
            km = 0;
        dist[i] = km;
    }
    qsort(dist, N, sizeof *dist, float_desc);
    for (int i = 0; i < N; ++i) {
        const int d = (int)trunc(dist[i]);
        const int count = i + 1;
        const int need = d - count;
        if (need <= 10 && need >= -10)
            printf("dist=%d count=%d need=%d\n", d, count, need);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

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
    int bin[20] = {0};
    for (int i = 0; i < 750; ++i) {
        double km = gaussian(100, 25);
        int j = (int)round(km / 10.0);
        if (j <  0) j =  0;
        if (j > 19) j = 19;
        bin[j]++;
        // printf("%2d %5.1f => %2d\n", i, km, j);
    }
    for (int i = 0; i < 20; ++i) {
        printf("%2d ", i);
        for (int j = 0; j < bin[i]; ++j)
            putc('*', stdout);
        putc('\n', stdout);
    }
    return 0;
}

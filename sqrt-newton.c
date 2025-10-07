/**
 * Square root:
 *     sqrt(a) = x
 *     => x^2 = a
 *     => f(x) = x^2 - a
 *             = 0
 * Newton-Raphson:
 *     x_n+1 = x_n - f(x_n) / f'(x_n)
 *           = x_n - (x_n ^2 - a) / (2.x_n)
 *           = (x_n ^2 + a)/(2.x_n)
 *           = (x_n + a/x_n)/2
 */

#include <stdio.h>  // printf, fgets, sscanf
#include <math.h>   // fabs

static double ask_dbl_pos(const char *msg)
{
    char buf[BUFSIZ];
    double x;
    do {
        do {
            printf("%s: ", msg);
            fgets(buf, sizeof buf, stdin);
        } while (sscanf(buf, "%lf", &x) != 1);
    } while (x <= 0);
    return x;
}

int main(void)
{
    const double frac = 0.00001;
    double val = ask_dbl_pos("Positive number");
    double root = ask_dbl_pos("First sqrt guess");
    double dif, thr;
    int i = 0;
    do {
        const double next = (root + val / root) / 2;
        dif = fabs(next - root);
        thr = fabs(frac * root);
        root = next;
        ++i;
    } while (dif >= thr);
    printf("After %d steps: sqrt(%f) = %f\n", i, val, root);
    return 0;
}

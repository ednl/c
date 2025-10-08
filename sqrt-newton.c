/**
 * Find square root of a:
 *   sqrt(a) = x
 *   => x^2 = a
 *   => x^2 - a = 0
 * Solve f(x)=0 for f(x) = x^2 - a
 *
 * Newton-Raphson
 * https://en.wikipedia.org/wiki/Newton%27s_method
 * x_n+1 = x_n - f(x_n) / f'(x_n)
 *       = x_n - (x_n ^2 - a) / (2.x_n)
 *       = (x_n ^2 + a)/(2.x_n)
 *       = (x_n + a/x_n)/2
 */

#include <stdio.h>  // printf, fgets, sscanf
#include <math.h>   // fabs

static double ask_dbl_pos(const char *msg)
{
    char buf[BUFSIZ] = {0};
    double x = 0;
    for (int res = 0; res != 1 || x <= 0; ) {
        printf("%s: ", msg);
        if (fgets(buf, sizeof buf, stdin))
            res = sscanf(buf, "%lf", &x);
    }
    return x;
}

int main(void)
{
    const double prec = 1e-5;  // arbitrary precision cut-off
    double val  = ask_dbl_pos("Positive number");
    double root = ask_dbl_pos("First sqrt guess");
    int step = 0;
    for (double diff = 0, goal = 0; diff >= goal; ++step) {
        const double next = (root + val / root) / 2;
        diff = fabs(next - root);
        goal = fabs(prec * root);  // fabs unnecessary for sqrt function
        root = next;
    }
    printf("After %d steps: sqrt(%.1f) = %.8f\n", step, val, root);
    return 0;
}

/**
 * Find square root of a:
 *   sqrt(a) = x
 *   => x^2 = a
 *   => x^2 - a = 0
 * Solve f(x)=0 for f(x) = x^2 - a
 *
 * Newton-Raphson iteration
 * https://en.wikipedia.org/wiki/Newton%27s_method
 * x_n+1 = x_n - f(x_n) / f'(x_n)
 *       = x_n - (x_n ^2 - a) / (2.x_n)
 *       = (x_n ^2 + a)/(2.x_n)
 *       = (x_n + a/x_n)/2
 */

#include <stdio.h>  // printf, fgets, sscanf
#include <math.h>   // fabs, sqrt
#include <stdbool.h>

static bool ispos(const double x)
{
    return x > 0;
}

static double ask_dbl(const char *msg, bool (*isvalid)(const double))
{
    char buf[BUFSIZ] = {0};
    double x = 0;
    for (int res = 0; res != 1 || !isvalid(x); ) {
        printf("%s : ", msg);
        if (fgets(buf, sizeof buf, stdin))
            res = sscanf(buf, "%lf", &x);
    }
    return x;
}

int main(void)
{
    double val  = ask_dbl("Positive number ", ispos);
    double root = ask_dbl("First sqrt guess", ispos);
    const double ref = sqrt(val);  // library function
    const double prec = 1e-5;  // arbitrary cut-off
    int step = 0;
    for (double diff = 0, goal = 0; diff >= goal; ++step) {
        const double next = (root + val / root) / 2;
        diff = fabs(next - root);
        goal = prec * root;  // root is always positive
        root = next;
    }
    printf("Newton-Raphson   : %.15e (%d steps)\n", root, step);
    printf("math.h sqrt()    : %.15e\n", ref);
    printf("error            : %+.2e\n", root - ref);
    return 0;
}

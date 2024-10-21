/**
 * From a homework assignment posted in /r/C_Programming
 * https://www.reddit.com/r/C_Programming/comments/1g50p73/need_help_with_an_overflow_issue/
 *
 * Given the Taylor series expansion of:
 *   f(x) = log(x + sqrt(1 + x^2))
 * around x=0 as:
 *   S = x - (1/2) * (x^3 / 3) + (1/2 * 3/4) * (x^5 / 5) - (1/2 * 3/4 * 5/6) * (x^7 / 7) + ...
 * calculate the series sum for a given x and epsilon.
 *
 * Observe that each term T(n) of the partial sum S is:
 *   T(0) = x
 *   T(n) = T(n-1) * -1 * x^2 * (2n-1)^2 / (2n * (2n+1))
 * for n=1,2,3,...
 *
 * Partial fraction decomposition of the extra coefficient factor per term:
 *      (2n-1)^2 / (2n(2n+1))
 *   = ((2n+1)^2 - 8n) / (2n(2n+1))
 *   = (2n+1)/(2n) - 8n / (2n(2n+1))
 *   = 1 + 1/(2n) - 4/(2n+1)
 * for n=1,2,3,...
 */

#include <stdio.h>   // printf
#include <stdlib.h>  // strtod, strtol
#include <math.h>    // log, hypot
#include <float.h>   // DBL_DECIMAL_DIG

static double epsilon(const int dec)
{
    return pow(10, -dec);
}

static double f(const double x)
{
    return log(x + hypot(x, 1));
}

static void progress(const int index, const double term, const double sum, const int dec)
{
    printf("%3d: %+.*f = %.*f\n", index, dec + 1, term, dec, sum);
}

static double estimate(const double x, const int dec)
{
    const double eps = epsilon(dec);
    double term = x;
    double sum = x;
    if (dec > 0 && dec <= DBL_DECIMAL_DIG)
        progress(0, term, sum, dec);

    const double mx2 = -x * x;
    for (int n = 1; fabs(term) >= eps && n < 1000; ++n) {
        const int n2 = n << 1;
        term *= mx2 * (1.0 / n2 - 4.0 / (n2 + 1) + 1);
        sum += term;
        if (dec > 0 && dec <= DBL_DECIMAL_DIG)
            progress(n, term, sum, dec);
    }
    return sum;
}

int main(void)
{
    char buf[BUFSIZ];

    printf("x? (|x|<=1) ");
    fgets(buf, sizeof buf, stdin);
    const double x = strtod(buf, NULL);
    if (fabs(x) > 1)
        return 1;

    printf("Decimal places? (1..%d) ", DBL_DECIMAL_DIG);
    fgets(buf, sizeof buf, stdin);
    const long d = strtol(buf, NULL, 10);
    if (d < 1 || d > DBL_DECIMAL_DIG)
        return 2;
    const int dec = (int)d;

    printf("\n");
    const double est = estimate(x, dec);  // also print progress
    printf("\n");

    printf("    x  = %.3f\n", x);
    printf("  f(x) = %.*f\n", dec, f(x));
    printf("taylor = %.*f\n", dec, est);
    printf("     +/- %.*f\n", dec, epsilon(dec));
}

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

#define MAXITER 1000

// Original analytic function.
static double f(const double x)
{
    return log(x + hypot(x, 1));
}

// Construct number 1e-n, e.g. for decimals=3: 0.001
static double make_epsilon(const int decimals)
{
    return pow(10, -decimals);
}

// Show the subsequent terms of the Taylor series.
static void progress(const int index, const double term, const double sum, const int decimals)
{
    printf("%3d: %+.*f = %.*f\n", index, decimals + 1, term, decimals, sum);
}

// Calculate the Taylor series sum for function f.
static double estimate(const double x, const int decimals)
{
    const double eps = make_epsilon(decimals);
    double term = x;
    double sum = x;
    if (decimals > 0 && decimals <= DBL_DECIMAL_DIG)
        progress(0, term, sum, decimals);

    const double mx2 = -x * x;
    for (int n = 1; !isinf(term) && !isinf(sum) && fabs(term) >= eps && n < MAXITER; ++n) {
        const int n2 = n << 1;
        term *= mx2 * (1.0 / n2 - 4.0 / (n2 + 1) + 1);
        sum += term;
        if (decimals > 0 && decimals <= DBL_DECIMAL_DIG)
            progress(n, term, sum, decimals);
    }
    return sum;
}

int main(void)
{
    char buf[BUFSIZ];

    printf("x? (|x|<=1) ");
    fgets(buf, sizeof buf, stdin);
    const double x = buf[0] == '\n' ? 0.5 : strtod(buf, NULL);
    if (fabs(x) > 1.001)
        return 1;

    printf("Decimal places? (1..%d) ", DBL_DECIMAL_DIG);
    fgets(buf, sizeof buf, stdin);
    const long d = buf[0] == '\n' ? 8 : strtol(buf, NULL, 10);
    if (d < 1 || d > DBL_DECIMAL_DIG)
        return 2;
    const int dec = (int)d;

    printf("\n");
    const double est = estimate(x, dec);  // also print progress
    printf("\n");

    printf("    x  = %.3f\n", x);
    printf("  f(x) = %.*f\n", dec, f(x));
    printf("taylor = %.*f\n", dec, est);
    printf("     +/- %.*f\n", dec, make_epsilon(dec));
}

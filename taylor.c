/**
 * From a homework assignment posted in /r/C_Programming
 * https://www.reddit.com/r/C_Programming/comments/1g50p73/need_help_with_an_overflow_issue/
 *
 * Given the Taylor series expansion of:
 *   f(x) = log(x + sqrt(1 + x^2))
 *
 * around x=0 as:
 *   S = x - (1/2) * (x^3 / 3) + (1/2 * 3/4) * (x^5 / 5) - (1/2 * 3/4 * 5/6) * (x^7 / 7) + ...
 *
 * calculate the series sum for a given x and epsilon.
 *
 * Partial fraction decomposition of the extra coefficient factor per term:
 *   (n-2)/((n-1)n) = A/(n-1) + B/n
 *   n-2 = An + B(n-1)
 *   n=1 => A = -1
 *   n=0 => B = 2
 *   => (n-2)/((n-1)n) = -1/(n-1) + 2/n
 */

#include <stdio.h>  // printf
#include <math.h>   // log, sqrt

static double f(const double x)
{
    return log(x + sqrt(1 + x * x));
}

static void progress(const int ord, const double coef, const double part, const double sum)
{
    printf("ord %2d coef %.12f part %+.12f sum %.12f\n", ord, coef, part, sum);
}

int main(void)
{
    // Input
    const double x = 20.0;
    const double eps = 1e-8;
    printf("x = %.3f eps = %.0e\nf(x) = %.12f\n\n", x, eps, f(x));

    // First term
    int ord = 1;
    double coef = 1.0;
    double part = x;
    double sum = x;
    progress(ord, coef, part, sum);

    // Next terms
    const double mx2 = -x * x;
    while (fabs(part) > eps) {
        ord += 2;
        coef *= 2.0 / ord - 1.0 / (ord - 1);  // see fraction decomposition above
        part *= mx2 * coef;
        sum += part;
        progress(ord, coef, part, sum);
    }

    return 0;
}

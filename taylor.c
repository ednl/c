#include <stdio.h>
#include <math.h>

static double f(const double x)
{
    return log(x + sqrt(1 + x*x));
}

static void progress(const int ord, const double coef, const double part, const double sum)
{
    printf("ord %2d coef %.12f part %+.12f sum %.12f\n", ord, coef, part, sum);
}

/**
 * Partial fraction decomposition of the extra coefficient factor per term:
 * (n-2)/((n-1)n) = A/(n-1) + B/n
 * n-2 = An + B(n-1)
 * n=1 => A = -1
 * n=0 => B = 2
 * => (n-2)/((n-1)n) = -1/(n-1) + 2/n
 */

int main(void)
{
    const double x = 20.0;
    const double epsilon = 1e-8;

    const double direct = f(x);
    printf("x = %.3f f(x) = %.12f\n", x, direct);

    int ord = 1;
    double coef = 1.0;
    double part = x;
    double sum = part;
    progress(ord, coef, part, sum);

    const double mx2 = -x * x;
    while (fabs(part) > epsilon) {
        ord += 2;
        coef *= 2.0 / ord - 1.0 / (ord - 1);  // see fraction decomposition above
        part *= mx2 * coef;
        sum += part;
        progress(ord, coef, part, sum);
    }

    return 0;
}

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

int main(void)
{
    const double x = 20.0;
    const double x2 = x * x;
    const double epsilon = 1e-8;

    const double direct = f(x);
    printf("x = %.3f f(x) = %.12f\n", x, direct);

    int ord = 1;
    double coef = 1.0;
    double part = x;
    double sum = part;
    progress(ord, coef, part, sum);

    while (fabs(part) > epsilon) {
        ord += 2;
        coef *= (double)(ord - 2) / ((ord - 1) * ord);
        part *= -x2 * coef;
        sum += part;
        progress(ord, coef, part, sum);
    }

    return 0;
}

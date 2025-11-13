#include <stdio.h>
#include <stdlib.h>  // size_t
#include <stdbool.h>

typedef struct ratio {
    size_t num, den;
    bool isneg;
} Ratio;

// Greatest Common Divisor
// https://en.wikipedia.org/wiki/Euclidean_algorithm
static size_t gcd(size_t a, size_t b)
{
	while (b) {
		const size_t tmp = b;
		b = a % b;
		a = tmp;
	}
	return a;
}

static Ratio ratio_nan(void)
{
    return (Ratio){0, 0, false};
}

static bool ratio_isnan(const Ratio r)
{
    return r.num == 0 && r.den == 0;
}

static Ratio ratio_zero(void)
{
    return (Ratio){0, 1, false};
}

static bool ratio_iszero(const Ratio r)
{
    return r.num == 0 && r.den != 0;
}

static Ratio ratio_inf(const bool isneg)
{
    return (Ratio){1, 0, isneg};
}

static bool ratio_isinf(const Ratio r)
{
    return r.num != 0 && r.den == 0;
}

static Ratio ratio_norm(const Ratio r)
{
    if (ratio_isnan(r) || ratio_iszero(r) || ratio_isinf(r))
        return r;
    const size_t c = gcd(r.num, r.den);
    return (Ratio){r.num / c, r.den / c, r.isneg};
}

static Ratio ratio_new_unsigned(const size_t num, const size_t den, const bool isneg)
{
    return ratio_norm((Ratio){num, den, isneg});
}

static Ratio ratio_new(const ssize_t num, const ssize_t den)
{
    return ratio_new_unsigned(num >= 0 ? num : -num, den >= 0 ? den : -den, (num < 0) != (den < 0));
}

static Ratio ratio_neg(const Ratio r)
{
    if (ratio_isnan(r))
        return ratio_nan();
    if (ratio_iszero(r))
        return ratio_zero();
    return (Ratio){r.num, r.den, !r.isneg};
}

static Ratio ratio_inv(const Ratio r)
{
    if (ratio_isnan(r))
        return ratio_nan();
    return (Ratio){r.den, r.num, r.isneg};
}

static Ratio rmul(const Ratio p, const Ratio q)
{
    if (ratio_isnan(p) || ratio_isnan(q))
        return ratio_nan();
    if (ratio_isinf(p) || ratio_isinf(q))
        return ratio_inf(p.isneg ^ q.isneg);
    const Ratio r1 = ratio_new_unsigned(p.num, q.den, false);
    const Ratio r2 = ratio_new_unsigned(q.num, p.den, false);
    return (Ratio){r1.num * r2.num, r1.den * r2.den, p.isneg ^ q.isneg};
}

static Ratio rdiv(const Ratio p, const Ratio q)
{
    return rmul(p, ratio_inv(q));
}

static Ratio radd(const Ratio p, const Ratio q)
{
    const size_t c = gcd(p.den, q.den);
    const size_t n1 = p.num * (q.den / c);
    const size_t n2 = q.num * (p.den / c);
    const size_t d = p.den / c * q.den;
    if (p.isneg == q.isneg)
        return ratio_new_unsigned(n1 + n2, d, p.isneg);
    if (n1 == n2)
        return ratio_new_unsigned(0, 1, false);
    if (q.isneg) {
        if (n1 >= n2)
            return ratio_new_unsigned(n1 - n2, d, false);
        return ratio_new_unsigned(n2 - n1, d, true);
    }
    if (n1 >= n2)
        return ratio_new_unsigned(n1 - n2, d, true);
    return ratio_new_unsigned(n2 - n1, d, false);
}

static void rprint(const Ratio r)
{
    if (!r.num && !r.den)
        printf("NaN");
    else if (!r.num)
        printf("0");
    else if (!r.den) {
        fputc(r.isneg ? '-' : '+', stdout);
        printf("Inf");
    } else {
        if (r.isneg)
            fputc('-', stdout);
        if (r.den == 1)
            printf("%zu", r.num);
        else
            printf("%zu/%zu", r.num, r.den);
    }
}

int main(void)
{
    rprint(rmul(ratio_new(105, 15), ratio_new(30, -33)));
    printf("\n");
	return 0;
}

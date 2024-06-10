// Definition 1: N is "mortar" if every 1..n is a divisor of N, where n = floor(sqrt(N)).
// Definition 2: the Least Common Multiple of 1..n is the smallest number N where every 1..n is a divisor of N.
// Equivalent to definiton 2: LCM(a,b) = a.b/GCD(a,b) where GCD(a,b) is the greatest common divisor of a and b
//   and LCM(a,b,c) = LCM(LCM(a,b),c) for any a,b,c.
// From definition 1: if n = floor(sqrt(N)) then n^2 <= N < (n+1)^1.
// From definition 1 and 2: if N is "mortar" then LCM(1..n) must be <= N.
// With n as input variable, the LCM grows exponentionally and N quadratically.
// So, once LCM(1..n) > N, N or any greater number can no longer be "mortar".

#include <stdio.h>
#include <stdbool.h>

// Greatest Common Divisor
// https://en.wikipedia.org/wiki/Euclidean_algorithm#Implementations
static unsigned gcd(unsigned a, unsigned b)
{
	while (b) {
		unsigned tmp = b;
		b = a % b;
		a = tmp;
	}
	return a;
}

// Least Common Multiple
// https://en.wikipedia.org/wiki/Least_common_multiple#Calculation
static unsigned lcm(unsigned a, unsigned b)
{
	return a / gcd(a, b) * b;
}

// https://www.reddit.com/r/math/comments/1dc4nfx/can_anyone_prove_24_is_not_the_largest_number/
static bool mortar(unsigned N, unsigned n)
{
    return n > 1 ? !(N % n) && mortar(N, n - 1) : true;
}

int main(void)
{
    for (unsigned n = 1, N = 1, prev = 1, l = 1; prev <= l; ++n, N += (n << 1) - 1, prev = l, l = lcm(l, n))
        printf("n=floor(sqrt(%3u..%3u))=%2u  lcm(1..%2u)=%10u\n", N, N + (n << 1), n, n, l);

    printf("\nMortar numbers:\n");
    for (unsigned n = 1, sqroot = 1, l = 1, nextsq = 1; l <= n; l = lcm(l, ++sqroot))
        for (nextsq += (sqroot << 1) + 1; n < nextsq; ++n)
            if (mortar(n, sqroot))
                printf("%d\n", n);

    return 0;
}

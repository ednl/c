// Approximate pi with the Leibniz sum.
//
// Leibniz says: pi/4 = 1 - 1/3 + 1/5 - 1/7 + 1/9 - ...
// which is analogous to a series expansion of arctan by Madhava, 14th C.
// See https://en.wikipedia.org/wiki/Leibniz_formula_for_π

// Make the sum monotonically rising instead of alternating,
// for faster approximation:
// pi = 4 sum[0..k]((-1)^k / (2k+1))      (Leibniz)
//    = 4 sum[0..n](1/(4n+1) - 1/(4n+3))  (combine 2 terms)
//    = 4 sum[0..n](2 / ((4n+1)(4n+3)))   (combine the fractions)
//    = 4 sum[0..n](2 / d[n])             where: d[n] = (4n+1)(4n+3)
//    = sum[0..n](8 / d[n])               (bring the factor in the sum)
//    = sum[0..n](1 / div[n])             where: div[n] = d[n]/8

// Denominators:
// d[n]   = (4n+1)(4n+3)
//        = 16n^2 + 16n + 3
// d[n+1] = (4(n+1) + 1)(4(n+1) + 3)
//        = (4n+5)(4n+7)
//        = 16n^2 + 48n + 35
//        = d[n] + 32n + 32
//        = d[n] + 32(n+1)

// Denominators divided by 8:
// div[n]   = d[n]/8
// div[0]   = d[0]/8
//          = 3/8                         (1)
// div[n+1] = d[n+1]/8
//          = (d[n] + 32(n+1))/8
//          = d[n]/8 + 4(n+1)
//          = div[n] + 4(n+1)             (2)

// Per term of the summation:
// s[n]   = 1/div[n]
// s[n+1] = 1/div[n+1]                    (3)

// Iterative sum:
// sum[0..n+1] = sum[0..n] + s[n+1]       (4)

// So, the new sum is the old sum plus the new term (eq. 4),
// where the new term is 1 over the new divisor (eq. 3),
// where the new divisor is the old divisor plus 4 times the new n (eq. 2),
// and the first divisor is 3/8 (eq. 1).

// Variable substitution:
// m[n]   = 4(n+1)                        (5)
// m[0]   = 4(0+1)
//        = 4                             (6)
// m[n+1] = 4((n+1)+1)
//        = 4n+8
//        = 4(n+1) + 4
//        = m[n] + 4                      (7)

#include <stdio.h>
#include <tgmath.h>  // atanl

int main(void)
{
	// This is not only the value, but also
	// the function that we are approximating.
	const long double pi = atanl(1) * 4;

	// This assumes ~21 significant digits.
	// Check LDBL_DIG in float.h for the actual value.
	printf("  pi = %25.21Lf\n", pi);

	// Initial values.
	unsigned long long int m = 4;                    // see eq. 6
	long double prev = -1, sum = 0, div = 3.0L / 8;  // see eq. 1

	// The divisor is a floating point value and will always keep growing
	// (even if ++n * 4 overflows) or stay the same (if ++n overflows,
	// or if ++n * 4 is smaller than the significant digits in div, which
	// will occur sooner than div overflowing itself).
	//
	// The new term added to the sum is the inverse of the divisor, which
	// will always keep getting smaller (or stay the same) but will never
	// get negative. So, the sum will always grow (or stay the same).
	//
	// Eventually there won't be enough significant digits in the sum to
	// notice the new term, or the new term will be zero. So then, the
	// previous sum and the new sum will be the same and the loop ends.

	while (prev < sum) {    // while we're getting closer
		prev = sum;         // remember the previous sum
		sum += 1.0L / div;  // new sum, see eqs. 4, 3
		div += m;           // new divisor, see eqs. 2, 5
		m += 4u;            // new counter, see eq. 7
	}

	printf(" sum = %25.21Lf\n", sum);
	printf(" err = %+25.21Lf\n", sum - pi);
	printf("   m =   %llu\n", m);
	printf(" div =   %.3Lf\n", div);
	printf("part = %25.21Lf\n", 1.0L / div);
	return 0;
}

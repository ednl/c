///////////////////////////////////////////////////////////////////////////////
////
////  Pi Monte Carlo
////
////  (c) E. Dronkert <e@dronkert.nl>
////      https://github.com/ednl
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // printf
#include <stdlib.h>  // arc4random, NULL
#include <stdint.h>  // uint64_t, UINT64_MAX
#include <math.h>    // M_PI

///////////////////////////////////////////////////////////////////////////////

// Best random value 0 <= r <= 1
static long double rnd(void)
{
	uint64_t r;
	arc4random_buf(&r, sizeof(uint64_t));
	return (long double)r / UINT64_MAX;
}

int main(void)
{
	int i, j;
	uint64_t hit = 0, tot = 0;
	long double a, b, pi, err;

	for (i = 1; i <= 100; ++i) {
		for (j = 0; j < 1000000; ++j) {

			++tot;
			a = rnd();
			b = rnd();
			if (a*a + b*b <= 1)
				++hit;

		}
		pi = (long double)hit / tot * 4;
		err = pi - M_PI;
		printf("%3d %.10Lf %+.10Lf\n", i, pi, err);
	}
	return 0;
}

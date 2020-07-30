///////////////////////////////////////////////////////////////////////////////
////
////  Pi Monte Carlo
////
////  (c) E. Dronkert <e@dronkert.nl>
////      https://github.com/ednl
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // printf
#include <stdlib.h>  // srand, rand, RAND_MAX, NULL
#include <time.h>    // to seed random number generator
#include <math.h>    // hypot

///////////////////////////////////////////////////////////////////////////////

double dr(void)
{
	return rand() / (RAND_MAX + 1.);
}

int main(void)
{
	unsigned long i, j, hit = 0, tot = 0;

	// Seed random number generator.
	srand(time(NULL));

	for (i = 0; i < 100; ++i) {
		for (j = 0; j < 1000000; ++j) {

			++tot;
			if (hypot(dr(), dr()) < 1.0)
				++hit;

		}
		printf("%.12f\n", (double)hit / tot * 4);
	}

	return 0;
}

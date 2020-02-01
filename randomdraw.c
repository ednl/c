///////////////////////////////////////////////////////////////////////////////
////
////  Raffle tickets: draw all numbers in a range in random order.
////
////  (c) E. Dronkert <e@dronkert.nl>
////      https://github.com/ednl
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // printf
#include <stdlib.h>  // srand, rand, RAND_MAX, NULL
#include <time.h>    // to seed random number generator

#define FIRST (1u)   // first number in the raffle ticket range
#define LAST  (9u)   // last number in the raffle ticket range
#define LEN   (LAST - FIRST + 1u)  // number of raffle tickets

///////////////////////////////////////////////////////////////////////////////

int main(void)
{
	unsigned int i, n, raffles = 10, ticket[LEN];

	// Seed random number generator.
	srand(time(NULL));

	// Do a certain number of complete raffles.
	while (raffles--)
	{
		// Initialize the range.
		for (n = 0; n < LEN; ++n)
			ticket[n] = FIRST + n;
		// n is now equal to LEN.

		// Draw all tickets in random order.
		// precondition: n == LEN
		while (n)
		{
			// Random index from range [0..n> with decreasing length = n to 1.
			// Unbiased, see https://en.cppreference.com/w/c/numeric/random/rand
			i = rand() / ((RAND_MAX + 1u) / n--);

			// Show the ticket number.
			// Amend format when using larger numbers than single digits.
			printf("%u", ticket[i]);

			// Swap last element of currently used array to index we just used
			// (not really *swap* because we decrease the array length anyway,
			// so no need to save ticket[i]).
			// No effect if indices i and n are equal, which is good.
			ticket[i] = ticket[n];
		}

		// End of the raffle.
		printf("\n");
	}
	return 0;
}

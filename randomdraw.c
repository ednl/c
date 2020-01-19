#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const unsigned int n1 = 1u, n2 = 9u;
static const unsigned int len = n2 - n1 + 1u;
static unsigned int ticket[len];

int main(void)
{
	unsigned int i, j, k;

	// Seed random number generator.
	srand(time(NULL));

	// Number of raffles.
	for (k = 0; k < 10; ++k)
	{
		// Init
		for (i = 0; i < len; ++i)
			ticket[i] = n1 + i;

		// Draw all tickets in random order.
		// precondition: i == len
		while (i)
		{
			// Random index from decreasing length array from i to 1.
			// Unbiased, see https://en.cppreference.com/w/c/numeric/random/rand
			j = rand() / ((RAND_MAX + 1u) / i);

			// Show the ticket number.
			printf("%u", ticket[j]);

			// Swap last element of currently used array to index we just used
			// (not really *swap* because we decrease the array length anyway,
			// so no need to save ticket[j]).
			// No effect if index j and --i are equal, which is good.
			ticket[j] = ticket[--i];
		}

		// New game
		printf("\n");
	}
	return 0;
}

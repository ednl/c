#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const unsigned int n1 = 1u, n2 = 9u;
static const unsigned int len = n2 - n1 + 1u;
static unsigned int draw[len];

int main(void)
{
	unsigned int i, j, k;

	// Seed
	srand(time(NULL));

	// Number of rounds
	for (k = 0; k < 10; ++k)
	{
		// Init
		for (i = 0; i < len; ++i)
			draw[i] = n1 + i;

		// Draw
		for (i = 0; i < len; ++i)
		{
			// Random index from decreasing length array
			// unbiased, see https://en.cppreference.com/w/c/numeric/random/rand
			j = rand() / ((RAND_MAX + 1u) / (len - i));

			// Show the element drawn
			printf("%u", draw[j]);

			// Swap last element of currently used array to index we just used
			// (not really swap because we decrease the array length anyway)
			draw[j] = draw[len - 1 - i];
		}

		// Next round
		printf("\n");
	}

	return 0;
}

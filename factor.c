//
//  factor.c
//  factor
//
//  Compile as: gcc -lm -Ofast -std=c99 -pedantic -o factor factor.c
//          or: clang -std=gnu17 -pedantic -Weverything -Oz -o factor factor.c
//
//  Created by E. Dronkert on 2011-08-27, last update 2019-09-04.
//  Copyright 2011-2019 E. Dronkert. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <fenv.h>

#define introot(x) ((unsigned long long)llrintl(sqrtl((long double)(x))))

void printfactor(unsigned long long number, unsigned long long primefactor, unsigned int* power, unsigned long long* root)
{
	printf("%llu", primefactor);
	if (*power > 1) {
		printf("^%u", *power);
	}
	if (number > 1) {
		printf(" x ");
	}
	*power = 0;
	*root = introot(number);
}

int main (int argc, const char* argv[])
{
	// Round down when using rint()
	fesetround(FE_DOWNWARD);

	unsigned long long n = 0, r = 0, p = 3;
	unsigned int q = 0, factotal = 0, facprime = 0;
	clock_t t = clock();

	// Get integer n as command line argument, reset on fail, set r = integer root of n, print n
	if (argc > 1) {
		if (sscanf(argv[1], "%llu", &n) == 1) {
			r = introot(n);
		} else {
			n = 0;
		}
	}
	printf("\n\t%llu = ", n);

	// Special case n <= 1
	if (n <= 1) {

		factotal = 1;
		facprime = 1;
		printf("%llu", n);

	} else {

		// Factor out 2 from n, count multiplicity as q
		while (!(n % 2)) {
			n /= 2;
			++q;
		}

		// If found then update counters, print 2^q
		if (q) {
			factotal += q;
			++facprime;
			printfactor(n, 2, &q, &r);
		}

		// p = currently investigated factor (start from 3)
		// r = integer root of n (updated in function printfactor())
		// Loop until we pass the root
		while (p <= r) {

			// Factor out p from n, count multiplicity as q
			while (!(n % p)) {
				n /= p;
				++q;
			}

			// If found then update counters, print p^q
			if (q) {
				factotal += q;
				++facprime;
				printfactor(n, p, &q, &r);
			}

			// Next factor (skip even factors because we already factored out 2)
			p += 2;
		}

		// Print residue
		if (n > 1) {
			++factotal;
			++facprime;
			printf("%llu", n);
		}
	}

	// Summary
	printf("\n\tTotal number of factors: %u\n"\
		"\tNumber of unique prime factors: %u\n"\
		"\tCalculation time: %lf s\n\n",
		factotal, facprime, (double)(clock() - t) / CLOCKS_PER_SEC);
	return 0;
}

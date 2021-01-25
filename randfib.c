// Random Fibonacci Sequence
// https://en.wikipedia.org/wiki/Random_Fibonacci_sequence
// https://www.numberphile.com/videos/random-fibonacci-numbers

// gcc -Wall -Ofast -I/opt/local/include -L/opt/local/lib -lgmp randfib.c -o randfib

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "/opt/local/include/gmp.h"

int main(void)
{
    unsigned long long i = 0;
    mpz_t p, q, t;
    mpf_t f1, f2;
    mpz_init_set_si(p, 1);
    mpz_init_set_si(q, 1);
    mpz_init(t);
    mpf_init(f1);
    mpf_init(f2);

    srand(time(NULL));

    while(i < (1ull << 20)) {  // ~10M
        mpz_set(t, p);
        if (rand() / ((RAND_MAX + 1u) / 2)) {
            mpz_add(p, p, q);
        } else {
            mpz_sub(p, p, q);
        }
        mpz_set(q, t);

        if (!(++i % 1000000ull)) {
            mpf_set_z(f1, p);
            mpf_set_z(f2, q);
            mpf_div(f1, f1, f2);
            printf("%12llu %9.3lf\n", i, mpf_get_d(f1));
        }
    }

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(t);
    mpf_clear(f1);
    mpf_clear(f2);
    return 0;
}

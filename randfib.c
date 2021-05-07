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
    mpz_t p, q, t;
    mpf_t f1, f2;
    mpz_init_set_si(p, 1);
    mpz_init_set_si(q, 1);
    mpz_init(t);
    mpf_init(f1);
    mpf_init(f2);

    srand(time(NULL));

    for (unsigned int i = 0; i < 10; ++i) {
        for (unsigned int j = 0; j < 1000000; ++j) {
            mpz_set(t, p);
            if (rand() / ((RAND_MAX + 1u) / 2)) {
                mpz_add(p, p, q);
            } else {
                mpz_sub(p, p, q);
            }
            mpz_set(q, t);
        }

        mpf_set_z(f1, p);
        mpf_set_z(f2, q);
        mpf_div(f1, f1, f2);
        printf("%3u %9.3lf\n", i, mpf_get_d(f1));
    }

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(t);
    mpf_clear(f1);
    mpf_clear(f2);
    return 0;
}

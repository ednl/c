#include <stdio.h>
#include <stdlib.h>    // rand
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <math.h>      // exp2, fma
#include "./startstoptimer.h"

#define LOOP (1000 * 1000)
static int8_t n[LOOP];

// n'th Fibonacci number from golden ratio approximation
#define L2PHI  0.6942419136306174  // log2((1+sqrt(5))/2)
#define NL2S5 -1.1609640474436813  // -log2(sqrt(5))
static int64_t fib_gr(int n)
{
    return llround(exp2(fma(n, L2PHI, NL2S5)));
}

// n'th Fibonacci number from iteration
static int64_t fib_it(int n)
{
    int64_t a = 0, b = 1;
    for (; n > 1; n -= 2) {
        a += b;
        b += a;
    }
    return n ? b : a;
}

static double run(int64_t (*f)(int))
{
    int64_t s = 0;
    starttimer();
    for (int i = 0; i < LOOP; ++i)
        s += f(n[i]);
    const double t = stoptimer_us();
    printf("%"PRId64"\n", s);
    return t;
}

int main(int argc, char *argv[])
{
    if (argc != 2 || (argv[1][0] != 'i' && argv[1][0] != 'g')) {
        fprintf(stderr,
            "Useage: %s [i,f]\n"
            "  i = iterative method\n"
            "  g = golden ratio approximation\n", argv[0]);
        return 1;
    }
    for (int i = 0; i < LOOP; ++i)
        n[i] = 26 + rand() % 10;
    printf("fn: %.0f us\n", argv[1][0] == 'g' ? run(fib_gr) : run(fib_it));
}

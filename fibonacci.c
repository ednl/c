#include <stdio.h>
#include <stdlib.h>    // rand
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <math.h>      // exp2, fma
#include "./startstoptimer.h"

#define LOOP (1000 * 1000)
#define RANGE   10
#define MINBASE 0
#define MAXBASE 59  // fib(69) is different for _gr and _it

static int8_t n[LOOP];

static void usage(const char *fname)
{
    fprintf(stderr,
        "Usage: %s g|i <%d..%d>\n"
        "  g = golden ratio approximation\n"
        "  i = iterative method\n"
        "  number %d..%d as base for fib(base+[0..%d])\n",
        fname, MINBASE, MAXBASE, MINBASE, MAXBASE, RANGE - 1);
    exit(1);
}

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

static void run(int64_t (*f)(int))
{
    int64_t s = 0;
    starttimer();
    for (int i = 0; i < LOOP; ++i)
        s += f(n[i]);
    const double t = stoptimer_us();
    printf("%"PRId64"\n", s);
    printf("Time: %.0f us\n", t);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        usage(argv[0]);

    if ((argv[1][0] != 'g' && argv[1][0] != 'i'))
        usage(argv[0]);

    int base = atoi(argv[2]);
    if (base < MINBASE || base > MAXBASE)
        usage(argv[0]);

    // Generate list of random n for calculating fib(n) where n = base + [0..9]
    for (int i = 0; i < LOOP; ++i)
        n[i] = base + rand() % RANGE;

    run(argv[1][0] == 'g' ? fib_gr : fib_it);
}

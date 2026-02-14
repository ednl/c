#include <stdio.h>
#include <stdlib.h>    // rand
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <math.h>      // exp2, fma
#include "./startstoptimer.h"

#define LOOP (1000 * 1000)
#define RANGE   10
#define MINBASE 0
#define MAXBASE (72 - RANGE)  // fib(72) is different for _gr and _it

static int8_t index[LOOP];

// n'th Fibonacci number from golden ratio approximation
#define L2PHI  0.69424191363061730173879  // log2((1+sqrt(5))/2)
#define NL2S5 -1.16096404744368117393516  // -log2(sqrt(5))
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
        s += f(index[i]);
    const double t = stoptimer_us();
    printf("%"PRId64"\n", s);
    printf("Time: %.0f us\n", t);
}

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

int main(int argc, char *argv[])
{
    // for (int n = 65; n < 80; ++n)
    //     printf("%d: %"PRId64" %"PRId64"\n", n, fib_gr(n), fib_it(n));
    char type = 0;
    int base = -1;
    if (argc != 3
        || (((type = argv[1][0]) != 'g' && type != 'i'))
        || (base = atoi(argv[2])) < MINBASE
        || base > MAXBASE)
        usage(argv[0]);

    // Generate list of random n for calculating fib(n) where n = base + [0..9]
    for (int i = 0; i < LOOP; ++i)
        index[i] = base + rand() % RANGE;

    run(type == 'g' ? fib_gr : fib_it);
}

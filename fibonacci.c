#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "./startstoptimer.h"

#define LOOP (1000 * 1000 * 1000)
#define LOGPHI   0.4812118250596029
#define LOGSQRT5 0.8047189562170503

// n'th Fibonacci number from golden ratio approximation
static int64_t fib_fn(int n)
{
    return llround(exp(LOGPHI * n - LOGSQRT5));
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
    volatile int n = 35;
    int64_t s = 0;
    starttimer();
    for (int i = 0; i < LOOP; ++i)
        s += f(n);
    const double t = stoptimer_ms();
    printf("%"PRId64"\n", s);
    return t;
}

int main(void)
{
    printf("fn: %.0f ms\n", run(fib_fn));
    printf("it: %.0f ms\n", run(fib_it));
}

#include <stdio.h>  // printf
#include <math.h>   // log, exp, ceil

// Assignment: find the second (N=2) number that can be expressed as a^3 + b^3 in two (M=2) different ways.
#define N 2U  // find N solutions
#define M 2U  // multiplicity = how many different ways

typedef struct {
    unsigned int a, b;
} Solution;

// Remember intermediate results
static Solution sol[M] = {0};

// (x-1)^(1/3) rounded up = upper bound for b where x = 1^3 + b^3
// NB: x > 1 because log(x - 1)
static unsigned int taxicabmax(unsigned int x)
{
    double r = ceil(exp(log(x - 1) / 3));
    return (unsigned int)r;
}

int main(void)
{
    unsigned int x = 2;  // first number 'x' to be explored must be greater than 1
    unsigned int n = 0;  // number of solutions found so far

    while (n < N) {
        unsigned int a = 1;
        unsigned int b = taxicabmax(x);
        unsigned int sum = 1 + b * b * b;
        unsigned int m = 0;

        while (a <= b) {
            // Initially: sum >= x, so start with greater-than test
            // a <= b, meaning 'b' jumps are at least as large as 'a' jumps, so this does not need to be a loop
            if (sum > x) {
                // (b - 1)^3 = (b^2 - b.a + 1)(b - 1) = b^3 - 2.b^2 + b - b^2 + 2.b - 1 = b^3 - 3.b^2 + 3.b - 1 = b^3 - (3.b.(b - 1) + 1)
                // => sum(a, b - 1) = sum(a, b) - (3.b.(b - 1) + 1)
                sum -= 3 * b * (b - 1) + 1;
                b -= 1;
            }
            // a <= b, meaning it might be necessary to increase 'a' multiple times, so this must be a loop
            while (sum < x) {
                // (a + 1)^3 = (a^2 + 2.a + 1)(a + 1) = a^3 + 2.a^2 + a + a^2 + 2.a + 1 = a^3 + 3.a^2 + 3.a + 1 = a^3 + 3.a.(a + 1) + 1
                // => sum(a + 1, b) = sum(a, b) + 3.a.(a + 1) + 1
                sum += 3 * a * (a + 1) + 1;
                a += 1;
            }
            // Did we find a solution for x = a^3 + b^3?
            if (sum == x) {
                sol[m++] = (Solution){a, b};  // save for later
                // Prepare to find next solution
                // Both increase 'a' and decrease 'b' to avoid f(a+1,b)=f(a,b+1) in the next loop, which are not distinct solutions
                sum += 3 * (a * (a + 1) - b * (b - 1));
                a += 1;
                b -= 1;
            }
        }
        // Did we find at least M solutions for a^3 + b^3 = x?
        if (m >= M) {
            n++;
            printf("%u", x);
            while (m--) {
                printf(" = %u^3 + %u^3", sol[m].a, sol[m].b);
            }
            printf("\n");
        }
        x++;
    }
    return 0;
}

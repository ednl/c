// Programmeeropdracht voor eerstejaars natuurkunde aan de UvA
// via Ivo van Vulpen https://twitter.com/IvovanVulpen/status/1585907956071432198

#include <stdio.h>  // printf
#include <math.h>   // log, exp, ceil

// Assignment: find the second (N=2) number that can be expressed as a^3 + b^3 in two (M=2) different ways.
#define N     2U  // find N numbers
#define M     2U  // multiplicity = how many different ways
#define SIZE 16U  // solution stack size, 16 is more than what is possible for 64-bit unsigned int, see https://en.wikipedia.org/wiki/Taxicab_number#Known_taxicab_numbers

// Remember all solutions per number
typedef struct {
    unsigned int a, b;
} Solution;
static Solution sol[SIZE] = {0};

int main(void)
{
    unsigned int x = 2;  // first number 'x' to be explored must be greater than 1
    unsigned int n = 0;  // number of solutions found so far

    // Find N numbers
    while (n < N) {
        unsigned int a = 1;
        double r = ceil(exp(log(x - 1) / 3));  // (x-1)^(1/3) rounded up = upper bound for b where x = 1 + b^3
        unsigned int b = (unsigned int)r;
        unsigned int sum = 1 + b * b * b;
        unsigned int m = 0;

        // Function is symmetrical in 'a' and 'b', so solutions are distinct for a <= b
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
                if (m < SIZE)
                    sol[m++] = (Solution){a, b};  // save for later
                // Prepare to find next solution
                // Both increase 'a' and decrease 'b' to avoid f(a,a+1)=f(a+1,a) in the next loop, which are not distinct solutions
                sum += 3 * (a * (a + 1) - b * (b - 1));
                a += 1;
                b -= 1;
            }
        }

        // Did we find at least M solutions for x = a^3 + b^3?
        if (m >= M) {
            n++;  // one more number found
            printf("%u", x);
            while (m--)
                printf(" = %u^3 + %u^3", sol[m].a, sol[m].b);
            printf("\n");
        }

        // Next number to be explored
        x++;
    }

    return 0;
}

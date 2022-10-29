// Smallest number that can be expressed as sum of two cubes
// in two different ways is 1729 which is the Hardy-Ramanujan
// number, or taxicab number Ta(2).
// Assignment: find the second smallest number and its two sums.

#include <stdio.h>

int main(void)
{
    int n = 1729;
    while (1) {
        // n = a^3 + b^3 where a <= b for distinct sums
        // => max value of a is b => n = 2a^3 => a^3 = n/2
        int n2 = ++n / 2, a = 1, a3 = 1, aa = 0, bb = 0;
        while (a3 <= n2) {
            int b = a, sum = a3 * 2;
            while (sum <= n) {
                if (sum == n) {
                    if (aa) {
                        printf("%d = %d^3 + %d^3 = %d^3 + %d^3\n", n, aa, bb, a, b);
                        return 0;
                    }
                    aa = a;
                    bb = b;
                    break;
                }
                b++;
                sum = a3 + b * b * b;
            }
            a++;
            a3 = a * a * a;
        }
    }
}

#include <stdio.h>

static void Opgave2(void)
{
    int n = 1729;
    while (1) {
        int n2 = ++n / 2, a = 1, a3 = 1, aa = 0, bb = 0;
        while (a3 <= n2) {
            int b = a, b3 = a3;
            while (a3 + b3 <= n) {
                if (a3 + b3 == n) {
                    if (aa) {
                        printf("%d = %d^3 + %d^3 = %d^3 + %d^3\n", n, aa, bb, a, b);
                        return;
                    }
                    aa = a;
                    bb = b;
                    break;
                }
                b++;
                b3 = b * b * b;
            }
            a++;
            a3 = a * a * a;
        }

    }
}

int main(void)
{
    Opgave2();
    return 0;
}

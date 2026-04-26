// https://en.wikipedia.org/wiki/Dottie_number
// https://mathworld.wolfram.com/DottieNumber.html

#include <stdio.h>
#include <math.h>

int main(void)
{
    double x = 0, prev = -1;
    printf("%3d: %.16f\n", 0, x);
    for (int i = 1; x != prev; ++i) {
        prev = x;
        x = cos(x);
        printf("%3d: %.16f\n", i, x);
    }
}

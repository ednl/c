/**
 * a + bc = 2024
 * ab + c = 2023
 * non-negative integer solutions {a,b,c}:
 *   {2024,0,2023}
 *   {674, 2, 675}
 *
 * a and c expressed in b:
 *   a = 2024 - bc = 2024 - b(2023 - ab) = 2024 - 2023b + ab^2
 *   a = (2023b - 2024) / (b^2 - 1)
 *   c = (2024b - 2023) / (b^2 - 1)
 * Maximum b for integer solution:
 *   b^2 - 2023b + 2023 = 0 => b = (2023 ± sqrt(2023^2 - 4*2023)) / 2 ~= 2022
 *   b^2 - 2024b + 2022 = 0 => b = (2024 ± sqrt(2024^2 - 4*2022)) / 2 ~= 2023
 */

#include <stdio.h>
int main(void)
{
    for (int b = 0; b < 2025; ++b) {
        // Numerators for a and c
        const int na = 2023 * b - 2024;
        const int nc = 2024 * b - 2023;
        // Denominator
        const int d = b * b - 1;
        // Remainders for a and c
        const int ra = na % d;
        const int rc = nc % d;
        if (!ra && !rc)
            // Solution found
            printf(">>> b=%d a=%d c=%d\n", b, na / d, nc / d);
        else if (ra <= 1 || rc <= 1)
            // Almost solution found for at least one part
            printf("b=%4d: %7d %% %7d = %7d ; %7d %% %7d = %7d\n", b, na, d, ra, nc, d, rc);
    }
}

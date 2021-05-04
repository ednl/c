#include <stdio.h>

int main(void)
{
    const char *inp[] = {
        "a,1,2,3",
        "   b   ,   4   ,   5   ,   6   ",
        "c,123456789012345678901,8,9",
        "d,10,11,a",
        "e,13,a,15",
        "f,a,17,18",
        "ghij, 21, 21",
    };
    for (int i = 0; i < sizeof inp / sizeof *inp; ++i) {
        // printf("\"%s\"\n", inp[i]);
        char buf[4];
        int a[3] = {0};
        int match = sscanf(inp[i], " %3[^,] ,%i ,%i ,%i", buf, &a[0], &a[1], &a[2]);
        if (match > 0) {
            printf("\"%s\": ", buf);
            for (int j = 1; j < match; ++j) {
                printf("%i=%i ", j, a[j-1]);
            }
        }
        printf("\n");
    }
    return 0;
}

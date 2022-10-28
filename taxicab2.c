#include <stdio.h>

#define N 100U

static unsigned int sum[N][N] = {0};

int main(void)
{
    // Store cube sums in array sum[i][j] for all i<=j<N
    for (unsigned int j = 1; j < N; ++j) {
        sum[0][j] = j * j * j;
    }
    for (unsigned int i = 1; i < N; ++i)
        for (unsigned int j = i; j < N; ++j)
            sum[i][j] = sum[0][i] + sum[0][j];

    unsigned int x = 1729, n = 0, startrow = 1;

    while (x < sum[N - 1][N - 1]) {
        unsigned int j = N - 1, m = 0, ii = 0, jj = 0;
        while (startrow < j && sum[startrow][j] < x)
            startrow++;
        unsigned int i = startrow;
        while (i <= j) {
            while (i < j && sum[i][j] > x)
                j--;
            if (i == j && sum[i][j] > x)
                break;
            while (i < j && sum[i][j] < x)
                i++;
            if (i == j && sum[i][j] < x)
                break;
            if (sum[i][j] == x) {
                if (m == 0) {
                    ii = i++;
                    jj = j--;
                    m = 1;
                } else {
                    printf("%u: %u = [%u,%u] = [%u,%u]\n", ++n, x, ii, jj, i, j);
                    break;
                }
            }
        }
        x++;
    }

    return 0;
}

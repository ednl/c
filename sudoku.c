#include <stdio.h>
#include <stdbool.h>

#define N 3
#define M (N * N)
static char board[M][M];

static bool isvalid(void)
{
    static const int d[M] = {0,0,0,1,1,1,2,2,2};  // i div 3
    unsigned row[M] = {0}, col[M] = {0}, blk[M] = {0};
    for (int i = 0; i < M; ++i) {      // rows
        for (int j = 0; j < M; ++j) {  // columns
            if (board[i][j] == '.') continue;  // skip empty square
            const char val = board[i][j] - '0';
            if (val < 1 || val > M) return false;  // wrong value
            const unsigned bit = 1u << val;  // set bit mask
            if (row[i] & bit) return false;  // already in row i?
            if (col[j] & bit) return false;  // already in column j?
            const int k = d[i] * N + d[j];   // block index 0..M-1
            if (blk[k] & bit) return false;  // already in block k?
            row[i] |= bit;  // set bit for row i
            col[j] |= bit;  // set bit for column j
            blk[k] |= bit;  // set bit for block k
        }
    }
    return true;
}

int main(void)
{
    return 0;
}

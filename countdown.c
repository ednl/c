#include <stdio.h>

#define CARDS 24
#define DRAWN 6
#define OPS   4

static const char opsym[OPS] = {'+', '-', 'x', '/'};
static const int card[CARDS] = {
    100,75,50,25,
    10,10,9,9,
    8,8,7,7,
    6,6,5,5,
    4,4,3,3,
    2,2,1,1,
};

static int draw[DRAWN];

// a >= 1, b >= 1
// op = [+,-,x,/]
static int res(const int a, const int b, const char op)
{
    if (a < b)
        return -1;
    if (a == 1)
        return op == '+' ? 2 : -1;
    if (b == 1) {
        switch (op) {
            case '+': return a + 1;
            case '-': return a - 1;
            default : return -1;
        }
    }
    if (a == b) {
        switch (op) {
            case '+': return a << 1;
            case 'x': return a * a;
            case '/': return 1;
            default : return -1;
        }
    }
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case 'x': return a * b;
        case '/': return a % b ? -1 : a / b;
    }
    return -1;
}

int main(void)
{
    int drw = 0x00140446;  // 75,50,9,7,3,2
    for (int i = 0, j = 0; drw && i < DRAWN && j < CARDS; ++i, ++j, drw >>= 1) {
        while (!(drw & 1)) {
            drw >>= 1;
            ++j;
        }
        draw[i] = card[j];
    }
    for (int i = 0; i < DRAWN - 1; ++i)
        printf("%d ", draw[i]);
    printf("%d\n", draw[DRAWN - 1]);
    return 0;
}

#include <stdio.h>

#define CARDS 24
static const int card[CARDS] = {
    1,1,2,2,
    3,3,4,4,
    5,5,6,6,
    7,7,8,8,
    9,9,10,10,
    25,50,75,100};
static int draw[6];

int main(void)
{
    int drw = 0x00611014;
    int i = 0, j = 0;
    while (drw && i < 6) {
        while (!(drw & 1)) {
            drw >>= 1;
            ++j;
        }
        drw >>= 1;
        draw[i++] = card[j++];
    }
    for (i = 0; i < 5; ++i)
        printf("%d ", draw[i]);
    printf("%d\n", draw[5]);
    return 0;
}

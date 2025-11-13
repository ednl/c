/**
 * From: https://www.reddit.com/r/C_Programming/comments/1ovqv0q/problem_with_scanf/
 * Write a C program that receives input in two lines.
 * The first line contains the number of integers (no more than 100).
 * The second line contains the integers separated by a space.
 * The program should read these integers, remove the negative numbers,
 * sort the even numbers in descending order, sort the odd numbers in ascending order,
 * and then display them with the even numbers first followed by the odd numbers.
 * Example:
 * Input:
 * 12
 * 1 2 3 -1 4 7 -4 6 3 12 15 14
 * Output:
 * 14 12 6 4 2 1 3 3 7 15
 * Note: When displaying the output, there is a space after the last number.
 */

#include <stdio.h>
#include <stdlib.h>

#define N 100
static int data[N];

static int cmp(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    const int odda = a & 1;
    const int oddb = b & 1;
    if (odda ^ oddb || a > b) return odda * 2 - 1;
    if (a < b) return (!odda) * 2 - 1;
    return 0;
}

int main(void)
{
    int inputlen;
    if (scanf("%d", &inputlen) != 1)
        return 1;
    if (inputlen < 1 || inputlen > N)
        return 2;
    int datalen = 0;
    for (int num; inputlen > 0 && scanf("%d", &num) == 1; --inputlen)
        if (num >= 0)
            data[datalen++] = num;
    qsort(data, datalen, sizeof *data, cmp);
    for (int i = 0; i < datalen; ++i)
        printf("%d ", data[i]);
    printf("\n");
}

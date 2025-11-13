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
#include <stdbool.h>

typedef enum myerr {
    E_OK = 0,  // no error
    E_INV1,    // error: invalid int value on line 1
    E_RANGE,   // error: value out of range on line 1
    E_INV2,    // error: invalid int value on line 2
} MyErr;

#define MAXLEN 100
static int data[MAXLEN];

// Map [0,1] -> [-1,+1]
static inline int asc(const bool state)
{
    return 2 * state - 1;
}

// Compare two ints according to spec. Return:
//   -1: sort a before b
//   +1: sort b before a
//    0: no action
static int spec(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    const bool aodd = a & 1;
    const bool bodd = b & 1;
    if (aodd ^ bodd || a > b) return asc( aodd);  // even before odd, odd: b before a, even: a before b
    if (               a < b) return asc(!aodd);  // odd: a before b, even: b before a
    return 0;  // a is b
}

int main(void)
{
    int inputlen;
    if (scanf("%d", &inputlen) != 1)
        return E_INV1;
    if (inputlen < 1 || inputlen > MAXLEN)
        return E_RANGE;
    int datalen = 0;
    for (int num; inputlen > 0; --inputlen) {
        if (scanf("%d", &num) != 1)
            return E_INV2;
        if (num >= 0)  // only save non-negative numbers
            data[datalen++] = num;
    }
    qsort(data, datalen, sizeof *data, spec);
    for (int i = 0; i < datalen; ++i)
        printf("%d ", data[i]);
    printf("\n");
    return EXIT_SUCCESS;
}

#include <stdio.h>

// Convert number into digit values, return digit count.
// Absolute value of x = sum of digits[i]*10^i for i=0..len-1
// For 32-bit int, array size of digits must be >=11
static int int2digits(char *digits, int x)
{
    const int base = 10;
    const int sign = x < 0 ? -1 : 1;
    int len = 0;
    do {
        digits[len++] = x % base * sign;
        x /= base;
    } while (x);
    return len;
}

// Print one digit value
static void putd(const char c)
{
    if (c >= 0 && c < 10)
        fputc('0' + c, stdout);
}

// Print array of digit values
static void printarray(const char *digits, const int len)
{
    printf("{");
    putd(digits[0]);
    for (int i = 1; i < len; ++i) {
        printf(",");
        putd(digits[i]);
    }
    printf("}\n");
}

// Print digit values as number
static void printdigits(const char *digits, int len)
{
    while (len > 0)
        putd(digits[--len]);
    printf("\n");
}

int main(void)
{
    char d[4 * sizeof (int)];
    int x = -123;
    int n = int2digits(d, x);
    printarray(d, n);   // "{3,2,1}"
    printdigits(d, n);  // "123"
    return 0;
}

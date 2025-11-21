#include <stdio.h>   // printf, fputc
#include <limits.h>  // INT_MIN

// Convert number into digit values in base 10, return digit count.
// Absolute value of x = sum of digits[i]*10^i for i=0..len-1
// For 32-bit int and base 10, array size of digits must be >=10
static int int2digits(char *digits, int x, const int base)
{
    if (base < 2 || base > 16)
        return 0;
    const int sign = x < 0 ? -1 : 1;
    int len = 0;
    do {
        digits[len++] = x % base * sign;
        x /= base;
    } while (x);
    return len;
}

// Print digit value from base 16 or below as digit character
static void putd(const char d)
{
    fputc("0123456789abcdef"[d & 15u], stdout);
}

// Print array of digit values as CSV
static void printarray(const char *digits, const int len)
{
    printf("{");
    if (len > 0)
        putd(digits[0]);
    for (int i = 1; i < len; ++i) {
        printf(",");
        putd(digits[i]);
    }
    printf("}\n");
}

// Print digit values as big-endian number
static void printnumber(const char *digits, int len)
{
    while (len > 0)
        putd(digits[--len]);
    printf("\n");
}

int main(void)
{
    char d[CHAR_BIT * sizeof (int)];     // room for base 2 representation
    const int x = INT_MIN;               // -x not possible as int
    const int n = int2digits(d, x, 10);  // n=10 with base 10
    printarray(d, n);                    // digits in reverse order as CSV
    printnumber(d, n);                   // normal, big-endian number
}

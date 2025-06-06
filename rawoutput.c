#include <stdio.h>   // fputc
#include <stdlib.h>  // div, div_t

#define MINBASE  2   // binary
#define MAXBASE 16   // hexadecimal
#define MAXLEN  32   // 32-bit int

// Output one byte, use your own hardware function here
// Return: 1=success, 0=failure
int hw_put(int c)
{
    return fputc(c, stdout) != EOF ? 1 : 0;
}

// Output a number (int, may be negative)
// Return: number of bytes successfully written
int printnum(int num, const int base, const int term)
{
    static const char digit[MAXBASE + 1] = "0123456789abcdef";
    if (base < MINBASE || base > MAXBASE)
        return 0;
    int len = 0;
    if (num < 0) {
        num = -num;
        len += hw_put('-');
    }
    char buf[MAXLEN];
    int i = 0;
    do {
        const div_t qr = div(num, base);
        buf[i++] = digit[qr.rem];
        num = qr.quot;
    } while (num > 0 && i < MAXLEN);
    while (i > 0)
        len += hw_put(buf[--i]);
    if (term >= 0)
        len += hw_put(term);  // change if you don't want to count the terminator
    return len;
}

int main(void)
{
    printnum(printnum(123456789, 16, '\n'), 10, '\n');
    return 0;
}

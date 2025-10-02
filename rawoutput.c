#include <stdio.h>   // fputc
#include <stdlib.h>  // div, div_t

#define MINBASE  2   // binary
#define MAXBASE 16   // hexadecimal
#define MAXLEN  32   // 32-bit int

static const char hexdigit[] = "0123456789abcdef";

// Output one byte, use your own hardware function here
// Return: 1=success, 0=failure
static int hw_put(int c)
{
    return fputc(c, stdout) != EOF;
}

// Output a number (int, may be negative)
// Terminate by outputting end if end >= 0
// Return: number of bytes successfully written, incl. end
static int printnum(int num, const int base, const int end)
{
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
        buf[i++] = hexdigit[qr.rem];
        num = qr.quot;
    } while (num > 0 && i < MAXLEN);
    while (i > 0)
        len += hw_put(buf[--i]);
    if (end >= 0)
        len += hw_put(end);  // change if you don't want to count the terminator
    return len;
}

static int printhex(unsigned num)
{
    char buf[8];  // 32-bit number = 8 hex digits
    int i = 0, len = 0;
    do {
        buf[i++] = hexdigit[num & 0xf];
        num >>= 4;
    } while (num && i < 8);
    while (i > 0)
        len += hw_put(buf[--i]);
    return len;
}

int main(void)
{
    printnum(printnum(123456789, 16, '\n'), 10, '\n');
    return 0;
}

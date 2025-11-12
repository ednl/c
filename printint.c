#include <stdio.h>    // printf
#include <unistd.h>   // write, STDOUT_FILENO
#include <stdlib.h>   // div, abs
#include <stdbool.h>  // bool

// Write decimal representation of signed integer 'x' to stdout.
// Does not rely on stdio.h, uses OS function 'write'.
// If 'term' != 0, append it.
// Sets global 'errno' if write fails.
// Return: sumber of bytes successfully written >= 0.
static ssize_t printint(int x, const char term)
{
    char buf[sizeof x * 4], *end = buf + sizeof buf, *pc = end;
    const bool neg = x < 0;
    if (term)
        *--pc = term;
	do {
        const div_t qr = div(x, 10);
		*--pc = '0' + abs(qr.rem);  // remainder of negative number is negative
		x = qr.quot;
	} while (x);
    if (neg)
        *--pc = '-';
    ssize_t n, sum = 0;
    while (pc < end && (n = write(STDOUT_FILENO, pc, end - pc)) != -1) {
        sum += n;
        pc += n;
    }
    return sum;
}

// Store decimal representation of signed integer 'x' in buffer 's' which
// must have enough space (max 12 chars for 32-bit INT_MIN, including sign).
// Return: pointer to next free character in s ('end' of number).
static char *sprintint(char *s, int x)
{
    char buf[sizeof x * 4], *pc = buf;
    const bool neg = x < 0;
	do {
        const div_t qr = div(x, 10);
		*pc++ = '0' + abs(qr.rem);  // remainder of negative number is negative
		x = qr.quot;
	} while (x);
    if (neg)
        *s++ = '-';
    while (pc > buf)
        *s++ = *--pc;
    return s;
}

int main(void)
{
    printint(123, '\n');
    printint(-321, '\n');

    char buf[32] = "x=";
    char *end = sprintint(buf + 2, -456);
    *end++ = '\n';
    *end++ = '\0';
    printf("%s", buf);
    return 0;
}

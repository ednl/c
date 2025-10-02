#include <stdio.h>   // fwrite
#include <stdlib.h>  // div

#define ALLOWNEGATIVE 1

static void printint(int x)
{
    char buf[16];  // 32-bit signed number in base 10 takes max 11 chars
    int i = sizeof buf;
#if ALLOWNEGATIVE
    char sign = 0;
    if (x < 0) {
        sign = '-';
        x = -x;
    }
#endif
	do {
        const div_t qr = div(x, 10);
		buf[--i] = qr.rem | '0';
		x = qr.quot;
	} while (x);
#if ALLOWNEGATIVE
    if (sign)
        buf[--i] = sign;
#endif
    fwrite(buf + i, sizeof buf - i, 1, stdout);
}

static char *sprintint(char *s, int x)
{
    char buf[16];
    int i = 0;
#if ALLOWNEGATIVE
    if (x < 0) {
        *s++ = '-';
        x = -x;
    }
#endif
	do {
        const div_t qr = div(x, 10);
		buf[i++] = qr.rem | '0';
		x = qr.quot;
	} while (x);
    while (i)
        *s++ = buf[--i];
    return s;
}

int main(void)
{
    printint(123);
    fputc('\n', stdout);
    printint(-321);
    fputc('\n', stdout);
    char buf[32] = "x=";
    char *end = sprintint(buf + 2, -456);
    *end = '\n';
    *(end + 1) = '\0';
    printf("%s", buf);
    return 0;
}

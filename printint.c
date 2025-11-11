#include <unistd.h>  // write, STDOUT_FILENO
#include <stdlib.h>  // div

static ssize_t printint(int x, const char term)
{
    char buf[sizeof x * 4], *end = buf + sizeof buf, *pc = end;
    const int sign = x < 0 ? -1 : 1;
    if (term)
        *--pc = term;
	do {
        const div_t qr = div(x, 10);
		*--pc = '0' + sign * qr.rem;  // remainder of negative number is negative
		x = qr.quot;
	} while (x);
    if (sign == -1)
        *--pc = '-';
    ssize_t n, sum = 0;
    while (end - pc > 0 && (n = write(STDOUT_FILENO, pc, end - pc)) != -1) {  // may set errno
        sum += n;
        pc += n;
    }
    return sum;
}

// static char *sprintint(char *s, int x)
// {
//     char buf[sizeof x * 4];
//     const int sign = x < 0 ? -1 : 1;
//     int i = 0;
// 	do {
//         const div_t qr = div(x, 10);
// 		buf[i++] = '0' + sign * qr.rem;  // remainder of negative number is negative
// 		x = qr.quot;
// 	} while (x);
//     if (sign == -1)
//         *s++ = '-';
//     while (i)
//         *s++ = buf[--i];
//     return s;
// }

int main(void)
{
    printint(123, '\n');
    printint(-321, '\n');
    // char buf[32] = "x=";
    // char *end = sprintint(buf + 2, -456);
    // *end = '\n';
    // *(end + 1) = '\0';
    // printf("%s", buf);
    return 0;
}

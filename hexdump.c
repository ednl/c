#include <stdio.h>   // printf
#include <string.h>  // strlen

// buf size must be greater than count*3
static char *dump(char *const buf, const unsigned char *bytes, int count)
{
    static const char *hex = "0123456789abcdef";
    if (!buf || !bytes || count < 1)
        return NULL;
    char *s = buf;
    while (count-- > 0) {
        *s++ = ' ';
        *s++ = hex[*bytes >> 4];
        *s++ = hex[*bytes++ & 0x0f];
    }
    *s = '\0';
    return buf;
}

static int min(const int a, const int b)
{
    return a < b ? a : b;
}

static int max(const int a, const int b)
{
    return a > b ? a : b;
}

int main(int argc, char *argv[])
{
    char buf[BUFSIZ];
    const int maxcount = (BUFSIZ - 1) / 3;

    int maxwidth = 0;
    for (int i = 1; i < argc; ++i)
        maxwidth = max(maxwidth, min(strlen(argv[i]), maxcount));

    for (int i = 1; i < argc; ++i)
        printf("%2d |%s | %-*s |\n", i, dump(buf, (unsigned char *)argv[i], maxwidth + 1), maxwidth, argv[i]);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>  // strtoul
#include <limits.h>  // INT_MAX

#define MAXHEXDIGITS (sizeof(unsigned) << 1)

static unsigned htoi(const char *s)
{
    if (!s)
        return 0;
    if (*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X'))
        s += 2;
    unsigned x = 0;
    for (size_t i = 0; i < MAXHEXDIGITS; ++i, ++s)
        if (*s >= '0' && *s <= '9')
            x = x << 4 | (*s & 0xf);
        else if ((*s >= 'A' && *s <= 'F') || (*s >= 'a' && *s <= 'f'))
            x = x << 4 | ((*s & 0x7) + 9);
        else
            return x;
    return x;
}

static unsigned htoi2(const char *s)
{
    unsigned long x = strtoul(s, NULL, 16);
    while (x > UINT_MAX)
        x >>= 4;
    return x;
}

int main(void)
{
    printf("0x%X\n", htoi("CafeBabe"));
    return 0;
}

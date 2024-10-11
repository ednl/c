#include <stdio.h>

static char *dump(char *const buf, const unsigned char *bytes, size_t count)
{
    static const char *hex = "0123456789abcdef";
    if (!buf || !bytes || !count)
        return NULL;
    char *s = buf;
    while (count--) {
        *s++ = hex[*bytes >> 4 & 0x0f];
        *s++ = hex[*bytes++    & 0x0f];
        *s++ = ' ';
    }
    *(s - 1) = '\n';
    *s = '\0';
    return buf;
}

int main(int argc, char *argv[])
{
    const size_t count = 8;
    char buf[BUFSIZ];  // size must be greater than count*3
    if (argc > 1)
        printf("%s", dump(buf, (unsigned char *)argv[1], count));
    return 0;
}

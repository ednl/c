#include <stdio.h>    // getline, sscanf, printf
#include <stdlib.h>   // free
#include <unistd.h>   // isatty, fileno
#include <stdbool.h>  // bool

static bool grow(int ** a, size_t * const size)
{
    static const size_t inc = 32;
    if (!a || !size)
        return false;
    size_t newlen = *size + inc;
    void *p = realloc(*a, newlen * sizeof **a);
    if (!p)
        return false;
    *a = p;
    *size = newlen;
    return true;
}

static size_t csv2int(int ** a, size_t * size, const char * const line)
{
    if (!a || !line || !*line)
        return 0;
    if (!*a && *size)
        *size = 0;
    const char *c = line;
    while (*c && *c != ',' && *c != ';')
        ++c;
    const char delim = *c == ';' ? ';' : ',';
    c = line;
    size_t i = 0;
    while (*c) {
        if (i >= *size && !grow(a, size))
            return i;
        int val;
        if (sscanf(c, "%d", &val) != 1)
            val = 0;
        (*a)[i++] = val;
        while (*c && *c != delim)
            ++c;
        if (*c == delim)
            ++c;
    }
    return i;
}

int main(int argc, char *argv[])
{
    int *a = NULL;
    size_t size = 0, range = 0;

    if (isatty(fileno(stdin))) {
        // No pipe or redirect, try command line argument
        if (argc > 1)
            range = csv2int(&a, &size, argv[1]);
    } else {
        // Input is pipe or redirect to stdin of this program
        char *buf = NULL;
        size_t bufsz;
        if (getline(&buf, &bufsz, stdin) > 0)
            range = csv2int(&a, &size, buf);
        free(buf);
    }

    if (a) {
        for (size_t i = 0; i < range; ++i)
            printf("%zu: %d\n", i, a[i]);
        free(a);
    }

    return 0;
}

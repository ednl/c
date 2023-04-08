#include <stdio.h>    // getline, sscanf, printf
#include <stdlib.h>   // free
#include <unistd.h>   // isatty, fileno
#include <stdbool.h>  // bool

static bool grow(int ** array, size_t * const len)
{
    static const size_t inc = 32;
    if (!array || !len)
        return false;
    size_t newlen = *len + inc;
    void *a = realloc(*array, newlen * sizeof **array);
    if (!a)
        return false;
    *array = a;
    *len = newlen;
    return true;
}

static size_t csv2int(int **array, size_t *len, const char * const line)
{
    if (!array || !line || !*line)
        return 0;
    if (!*array && *len)
        *len = 0;
    const char *c = line;
    while (*c && *c != ',' && *c != ';')
        ++c;
    const char delim = *c == ';' ? ';' : ',';
    c = line;
    size_t i = 0;
    while (*c) {
        if (i >= *len && !grow(array, len))
            return i;
        int val;
        if (sscanf(c, "%d", &val) != 1)
            val = 0;
        (*array)[i++] = val;
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
    size_t len = 0, last = 0;

    if (isatty(fileno(stdin))) {
        // No pipe or redirect, try command line argument
        if (argc > 1)
            last = csv2int(&a, &len, argv[1]);
    } else {
        // Input is pipe or redirect to stdin of this program
        char *buf = NULL;
        size_t bufsz;
        if (getline(&buf, &bufsz, stdin) > 0)
            last = csv2int(&a, &len, buf);
        free(buf);
    }

    if (a) {
        for (size_t i = 0; i < last; ++i)
            printf("%zu: %d\n", i, a[i]);
        free(a);
    }

    return 0;
}

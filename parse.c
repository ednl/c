#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strncmp
#include <ctype.h>   // isalpha, isdigit
#include <stddef.h>  // ptrdiff_t

static const char *cmddict[] = {
    "punt",
    "lijn",
    "rechthoek",
    "cirkel",
    "ellips",
};

static const char *input[] = {
    "cirkel 100 100 50 128",
    " lijn 0",
    "bla 1234 34 3456",
    "punt -1000 12342345567",
    "cirkel100x200_50+10 ",
};

static int parse(const char *line)
{
    const char *start = line, *stop;

    // Find first letter
    while (*start != '\0' && !isalpha(*start))
        ++start;
    if (!isalpha(*start))
        return 1;   // no command found

    // Find end of command
    stop = start;
    while (isalpha(*stop))
        ++stop;
    ptrdiff_t diff = stop - start;
    if (diff <= 0)
        return 2;  // empty command
    size_t len = (size_t)diff;

    // Detect command
    int cmdindex = -1;
    for (size_t i = 0; i < sizeof cmddict / sizeof *cmddict; ++i)
        if (!strncmp(start, cmddict[i], len))
        {
            cmdindex = (int)i;
            break;
        }
    if (cmdindex == -1)
        return 3;  // unknown command

    printf("Commando: %s\n", cmddict[cmdindex]);

    int paramcount = 0;
    while (*stop != '\r' && *stop != '\n' && *stop != '\0')
    {
        // Find first digit
        start = stop;
        while (*start != '\0' && *start != '+' && *start != '-' && !isdigit(*start))
            ++start;
        if (*start != '+' && *start != '-' && !isdigit(*start))
            break;   // no more params

        // Find last digit
        stop = start;
        if (*stop == '+' || *stop == '-')
        {
            ++stop;
            if (!isdigit(*stop))
                return 4;  // invalid param
        }
        while (isdigit(*stop))
            ++stop;
        diff = stop - start;
        if (diff <= 0)
            return 5;  // empty param
        len = (size_t)diff;

        // Test length
        if (len > 5)
            return 6;  // param overflow

        // Convert to int
        ++paramcount;
        int n = atoi(start);
        // TODO: test range
        printf("Argument %i: %i\n", paramcount, n);
    }
    // TODO: test paramcount against required number of params for this cmd
    return 0;
}

int main(void)
{
    for (size_t i = 0; i < sizeof input / sizeof *input; ++i)
        printf("Foutcode: %i\n\n", parse(input[i]));
    return 0;
}

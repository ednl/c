#include <stdio.h>
#include <stdlib.h>       // atoi, exit, EXIT_FAILURE
#include <stdint.h>       // uint64_t
#include <inttypes.h>     // PRIu64
#include <unistd.h>       // getopt, opterr, optind
#include <stdnoreturn.h>  // noreturn

#define MINBASE    2  // binary
#define MAXBASE   36  // 0-9,a-z
#define MAXLEN    19  // 10^19 < 2^64
#define MAXLOOP 1024  // gamble at a workable limit

typedef enum verbosity {
    SILENT, QUIET, NORMAL, VERBOSE, DEBUG
} Verbosity;

typedef struct cycle {
    struct cycle *prev, *next;
    int mu, lambda, count;
} Cycle;

static const char *digitchar = "0123456789abcdefghijklmnopqrstuvwxyz";
static Verbosity verbosity = NORMAL;

static void putnum(uint64_t x, const int base, const int len)
{
    char s[MAXLEN];
    int i = 0;
    while (i < len && x) {
        s[i++] = digitchar[x % base];
        x /= base;
    }
    while (i < len)
        s[i++] = '0';
    while (i--)
        putchar(s[i]);
}

// https://en.wikipedia.org/wiki/Kaprekar%27s_routine
static uint64_t kaprekar(uint64_t x, const int base, const int len)
{
    // Convert to array of digit values
    uint8_t digit[MAXLEN] = {0};
    for (int i = 0; i < len && x; ++i) {
        digit[i] = x % base;
        x /= base;
    }
    // Insertion sort
    for (int i = 1, j; i < len; ++i) {
        const uint8_t ins = digit[i];
        for (j = i; j && digit[j - 1] > ins; --j)
            digit[j] = digit[j - 1];
        if (j != i)
            digit[j] = ins;
    }
    // Return positive difference
    uint64_t diff = 0;
    for (int i = 0, j = len - 1; i < len; ++i, --j)
        diff = diff * base + digit[j] - digit[i];
    return diff;
}

static noreturn void usage(const char *const progname, const int exitcode)
{
    if (verbosity > SILENT) {
        fprintf(stderr, "Usage: %s [-sqvd] <base> <length>\n", progname);
        fprintf(stderr, "  %d <= base <= %d\n", MINBASE, MAXBASE);
        fprintf(stderr, "  1 <= length <= %d\n", MAXLEN);
        fprintf(stderr, "Options: s=silent, q=quiet, v=verbose, d=debug\n");
    }
    exit(exitcode);
}

int main(int argc, char *argv[])
{
    // Command line options.
    const char *const progname = argv[0];  // save program name
    opterr = 0;  // don't let getopt show error messages
    int ch;
    while ((ch = getopt(argc, argv, "sqvd")) != -1) {
        switch (ch) {
            case 's': verbosity = SILENT;  break;
            case 'q': verbosity = QUIET;   break;
            case 'v': verbosity = VERBOSE; break;
            case 'd': verbosity = DEBUG;   break;
            default: usage(progname, 1);  // exit with failure
        }
    }
    // Shift options out.
    argc -= optind;
    argv += optind;

    if (argc != 2)
        usage(progname, 2);
    int base = atoi(argv[0]);
    int len = atoi(argv[1]);
    if (base < MINBASE || base > MAXBASE || len < 1 || len > MAXLEN)
        usage(progname, 3);

    // Range = base^len
    uint64_t end = base;
    for (int i = 1; i < len; ++i)
        end *= base;

    // Cycle *cycle = NULL;
    for (uint64_t n = 0; n < end; ++n) {
        if (verbosity > SILENT)
            putnum(n, base, len);

        // https://en.wikipedia.org/wiki/Cycle_detection
        uint64_t x[MAXLOOP] = {n};  // tortoise: x0=n,x1=0,x2=0,x3=0,...
        uint64_t y[MAXLOOP];  // hare: x0,x2,x4,x6,...
        x[1] = kaprekar(n, base, len);
        y[1] = kaprekar(x[1], base, len);

        // Find repetition
        int i = 1;  // current tortoise & hare index
        while (i < MAXLOOP - 1 && x[i] != y[i]) {
            ++i;
            x[i] = (i & 1) ? kaprekar(x[i - 1], base, len) : y[i >> 1];  // single step tortoise
            y[i] = kaprekar(kaprekar(y[i - 1], base, len), base, len);   // double step hare
        }
        if (x[i] != y[i]) {
            if (verbosity > SILENT)
                printf("\n");
            if (verbosity == DEBUG)
                fprintf(stderr, "n=%"PRIu64" base=%d len=%d i=%d\n", n, base, len, i);
            exit(EXIT_FAILURE);
        }

        // Find start of loop
        int mu = 0;  // index of start of loop
        while (i < MAXLOOP - 1 && x[mu] != y[i]) {
            ++mu;
            ++i;
            y[i] = kaprekar(y[i - 1], base, len);  // single step hare
        }
        if (x[mu] != y[i]) {
            if (verbosity > SILENT)
                printf("\n");
            if (verbosity == DEBUG)
                fprintf(stderr, "n=%"PRIu64" base=%d len=%d mu=%d i=%d\n", n, base, len, mu, i);
            exit(EXIT_FAILURE);
        }

        // Find loop lenth
        i = mu + 1;  // mu + lambda = first inside the loop
        if (!x[i])
            x[i] = kaprekar(x[i - 1], base, len);  // first value inside loop
        while (i < MAXLOOP - 1 && x[mu] != x[i]) {
            if (!x[++i])
                x[i] = kaprekar(x[i - 1], base, len);
        }
        if (x[mu] != x[i]) {
            if (verbosity > SILENT)
                printf("\n");
            if (verbosity == DEBUG)
                fprintf(stderr, "n=%"PRIu64" base=%d len=%d mu=%d lambda=%d\n", n, base, len, mu, i - mu);
            exit(EXIT_FAILURE);
        }
        int lambda = i - mu;  // loop length

        // Cycle *c = cycle;
        // while (c && (c->lambda < lambda || (c->lambda == lambda && c->mu < mu)) && c->next)
        //     c = c->next;

        // Show startup and loop numbers
        if (verbosity >= VERBOSE)
            for (i = 1; i <= mu + lambda; ++i) {
                putchar(' ');
                putnum(x[i], base, len);
            }

        // Result
        if (verbosity > SILENT)
            printf(" (%d,%d)\n", mu, lambda);
    }
    return 0;
}

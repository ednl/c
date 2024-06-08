#include <stdio.h>
#include <stdlib.h>       // atoi, exit, EXIT_FAILURE
#include <string.h>       // memmove
#include <stdint.h>       // uint64_t
#include <inttypes.h>     // PRIu64
#include <unistd.h>       // getopt, opterr, optind
#include <stdnoreturn.h>  // noreturn

// Practical limits
#define MINBASE  2  // binary
#define MAXBASE 36  // 0-9,a-z
#define MAXLEN  19  // 10^19 < 2^64

// Guesses at workable limits
#define MAXLOOP  1024
#define MAXCYCLE 1024

// Defaults
#define BASE 10
#define LEN   4

typedef enum verbosity {
    SILENT, QUIET, NORMAL, VERBOSE, DEBUG
} Verbosity;

typedef struct cycle {
    int mu, lambda, count;
} Cycle;

static const char *digitchar = "0123456789abcdefghijklmnopqrstuvwxyz";
static Verbosity verbosity = NORMAL;

// Output number in any base 2..36, padded with zeroes to length len.
static void putnum(uint64_t n, const int base, const int len)
{
    char s[MAXLEN];
    int i = 0;
    while (i < len && n) {
        s[i++] = digitchar[n % base];
        n /= base;
    }
    while (i < len)
        s[i++] = '0';
    while (i--)
        putchar(s[i]);
}

// Next number in Kaprekar sequence in any base 2..36 with fixed length including zeroes.
// https://en.wikipedia.org/wiki/Kaprekar%27s_routine
static uint64_t kaprekar(uint64_t n, const int base, const int len)
{
    // Convert number to array of digit values.
    uint8_t digit[MAXLEN] = {0};
    for (int i = 0; i < len && n; ++i) {
        digit[i] = n % base;
        n /= base;
    }
    // Sort digits in ascending order with insertion sort.
    // https://en.wikipedia.org/wiki/Insertion_sort
    for (int i = 1, j; i < len; ++i) {
        const uint8_t insert = digit[i];
        for (j = i; j && digit[j - 1] > insert; --j)
            digit[j] = digit[j - 1];
        if (j != i)
            digit[j] = insert;
    }
    // Return positive difference between numbers
    // with digits in descending vs. ascending order.
    uint64_t diff = 0;
    for (int i = 0, j = len - 1; i < len; ++i, --j)
        diff = diff * base + digit[j] - digit[i];
    return diff;
}

// Explain how to use this program, and exit.
static noreturn void usage(const char *const progname, const int exitcode)
{
    if (verbosity != SILENT) {
        fprintf(stderr, "Usage: %s [-sqvd] [base] [length]\n", progname);
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

    int base = argc > 0 ? atoi(argv[0]) : BASE;
    int len  = argc > 1 ? atoi(argv[1]) : LEN;
    if (base < MINBASE || base > MAXBASE || len < 1 || len > MAXLEN)
        usage(progname, 2);

    // Range = base^len
    uint64_t end = base;
    for (int i = 1; i < len; ++i)
        end *= base;

    Cycle cycle[MAXCYCLE];
    int cyclecount = 0;

    for (uint64_t n = 0; n < end; ++n) {
        if (verbosity > SILENT)
            putnum(n, base, len);

        // https://en.wikipedia.org/wiki/Cycle_detection
        // Init
        uint64_t x[MAXLOOP] = {n};  // tortoise: x0=n,x1=0,x2=0,x3=0,...
        uint64_t y[MAXLOOP];        // hare: x0,x2,x4,x6,...
        x[1] = kaprekar(n, base, len);     // x1=f(x0)
        y[1] = kaprekar(x[1], base, len);  // x2=f(f(x0))

        // Find repetition
        int i = 1;  // current tortoise & hare index
        while (i < MAXLOOP - 1 && x[i] != y[i]) {
            ++i;
            x[i] = (i & 1) ? kaprekar(x[i - 1], base, len) : y[i >> 1];  // single step tortoise
            y[i] = kaprekar(kaprekar(y[i - 1], base, len), base, len);   // double step hare
        }
        if (x[i] != y[i]) {
            if (verbosity != SILENT)
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

        if (cyclecount) {
            i = 0;
            while (i < cyclecount && (cycle[i].mu < mu || (cycle[i].mu == mu && cycle[i].lambda < lambda)))
                ++i;
            if (i < cyclecount) {
                if (cycle[i].mu == mu && cycle[i].lambda == lambda)
                    cycle[i].count++;
                else {
                    //TODO: insert new entry
                    // memmove();
                }
            } else if (cyclecount < MAXCYCLE)
                cycle[cyclecount++] = (Cycle){mu, lambda, 1};
            else
                ;  //TODO: handle error
        } else
            cycle[cyclecount++] = (Cycle){mu, lambda, 1};

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

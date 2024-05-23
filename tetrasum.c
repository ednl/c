/******************************************************************************
 * Brady Haran of Numberphile asked: which 5 tetrahedral numbers sum to 343867?
 * This is my multi-threaded solution that finds 322 different combinations of
 * 5 tetrahedral numbers (277 all unique, 45 with duplicates).
 *
 * Compile:
 *   Mac: clang -std=gnu17 -Ofast -march=native -Wall -Wextra -o tetrasum tetrasum.c
 *   Linux: gcc -std=gnu17 -Ofast -march=native -Wall -Wextra -o tetrasum tetrasum.c -lm -lpthread
 *
 * Usage: tetrasum [-q|-s] [target]
 *   Default target=343867, show all sums with up to 5 terms, full summary.
 * Example: tetrasum -q 1234567
 *   (quiet) Show sums with up to 4 terms, full summary.
 * Example: tetrasum -s 10000001
 *   (silent) Only show terse summary with no. of sums per term count.
 *
 * Made by:
 *   E. Dronkert
 *   Utrecht, Netherlands
 *   https://github.com/ednl/c/blob/master/tetrasum.c
 *
 * Versions:
 *   2024-05-20 v1: First try with unique combinations.
 *   2024-05-23 v2: Now as requested with duplicates, and threads.
 *
 * Extra info:
 * Numberphile video is out now: https://www.youtube.com/watch?v=CK3_zarXkw0
 * Turns out this was in reference to one of Pollock's conjectures, see
 * https://en.wikipedia.org/wiki/Pollock%27s_conjectures and 343867 is thought
 * to be the last (largest) sum that needs five tetrahedral numbers. But they
 * don't have to be distinct! E.g. 17 is the first (smallest) that needs more
 * than four: 17 = 1 + 1 + 1 + 4 + 10.
 ******************************************************************************/

#if __APPLE__
    #include <sys/sysctl.h>  // sysctlbyname
#elif __linux__
    #define _GNU_SOURCE  // must come before all includes
    #include <sched.h>   // sched_getaffinity
#endif
#include <stdio.h>
#include <stdlib.h>    // malloc, free
#include <math.h>      // cbrt, floor
#include <stdint.h>    // uint32_t, UINT32_MAX
#include <inttypes.h>  // PRIu32
#include <string.h>    // strcmp
#include <unistd.h>    // getopt
#include <pthread.h>   // pthread_create, pthread_join

#define DEFTARGET   343867
#define MAXTHREADS  32
#define MAXSUMMANDS 5

typedef enum verbosity {
    SILENT, QUIET, NORMAL
} Verbosity;

// Pollock's conjecture: all positive whole numbers are sums
// of at most 5 tetrahedral numbers (duplicates allowed).
// First number that requires more than 4 terms is 17.
// Last number that requires more than 4 terms is probably 343867.
static uint32_t target = DEFTARGET;

// Array of tetrahedral numbers [0,1,4,10,20,35,...]
static uint32_t *tetrahedral;

static int threads, tetracount, duplicates;
static int summands[MAXSUMMANDS];  // a 'summand' is a term in a sum
static Verbosity verbosity = NORMAL;

// Help
static _Noreturn void usage(const char *filename)
{
    fprintf(stderr, "Tetrasum, by E. Dronkert, https://github.com/ednl\n");
    fprintf(stderr, "----------------------------------------------------------------------\n");
    fprintf(stderr, "Usage: %s [-q|-s] [target]\n", filename);
    fprintf(stderr, "  target must be greater than 0 and smaller than 2^32 (default=%d)\n", DEFTARGET);
    fprintf(stderr, "  -q = don't show sums with %d terms ('quiet')\n", MAXSUMMANDS);
    fprintf(stderr, "  -s = only show number of sums per term count ('silent')\n");
    fprintf(stderr, "----------------------------------------------------------------------\n");
    exit(EXIT_FAILURE);
}

// Number of CPU cores available to this program.
// Return: value between lo and hi, inclusive.
static int coresavail(const int lo, const int hi)
{
    int n = 0;
    #if __APPLE__
        size_t size = sizeof n;
        sysctlbyname("hw.activecpu", &n, &size, NULL, 0);
    #elif __linux__
        cpu_set_t set;
        CPU_ZERO(&set);
        sched_getaffinity(0, sizeof set, &set);
        n = CPU_COUNT(&set);
    #endif
    return n < lo ? lo : (n > hi ? hi : n);
}

// Parallel execution in separate threads.
static void *loop(void *arg)
{
    const int start = *(int *)arg;

    // First term
    for (int i = start; i < tetracount; i += threads) {
        uint32_t partial1 = tetrahedral[i];  // partial sum with 1 term
        if (partial1 == target) {
            ++summands[0];  // found a sum with 1 term (i.e. target is tetrahedral number)
            if (verbosity != SILENT)
                printf("%"PRIu32"\n", tetrahedral[i]);
        }

        // Second term
        for (int j = i; j < tetracount; ++j) {
            uint32_t partial2 = partial1 + tetrahedral[j];  // partial sum with 2 terms
            if (partial2 < partial1 || partial2 > target)
                break;  // overflow or impossible sum, proceed with next 1st term
            if (partial2 == target) {
                ++summands[1];  // found a sum with 2 terms
                duplicates += i == j;
                if (verbosity != SILENT)
                    printf("%"PRIu32" + %"PRIu32"\n", tetrahedral[i], tetrahedral[j]);
                break;
            }

            // Third term
            for (int k = j; k < tetracount; ++k) {
                uint32_t partial3 = partial2 + tetrahedral[k];  // partial sum with 3 terms
                if (partial3 < partial2 || partial3 > target)
                    break;  // overflow or impossible sum, proceed with next 2nd term
                if (partial3 == target) {
                    ++summands[2];  // found a sum with 3 terms
                    duplicates += i == j || j == k;
                    if (verbosity != SILENT)
                        printf("%"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetrahedral[i], tetrahedral[j], tetrahedral[k]);
                    break;
                }

                // Fourth term
                for (int l = k; l < tetracount; ++l) {
                    uint32_t partial4 = partial3 + tetrahedral[l];  // partial sum with 4 terms
                    if (partial4 < partial3 || partial4 > target)
                        break;  // overflow or impossible sum, proceed with next 3rd term
                    if (partial4 == target) {
                        ++summands[3];  // found a sum with 4 terms
                        duplicates += i == j || j == k || k == l;
                        if (verbosity != SILENT)
                            printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetrahedral[i], tetrahedral[j], tetrahedral[k], tetrahedral[l]);
                        break;
                    }

                    // To find a sum with 5 terms, residue must be a tetrahedral number.
                    uint32_t residue = target - partial4;
                    if (residue < tetrahedral[l] || residue > tetrahedral[tetracount - 1])
                        continue;  // 5th term is impossible, proceed with next 4th term

                    // Check edge cases.
                    if (residue == tetrahedral[l] || residue == tetrahedral[tetracount - 1]) {
                        int m = residue == tetrahedral[l] ? l : tetracount - 1;
                        ++summands[4];  // found a sum with 5 terms
                        duplicates += i == j || j == k || k == l || l == m;
                        if (verbosity == NORMAL)
                            printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetrahedral[i], tetrahedral[j], tetrahedral[k], tetrahedral[l], tetrahedral[m]);
                        continue;  // 5th term found, proceed with next 4th term
                    }

                    // Binary search for 5th term.
                    int lo = l, hi = tetracount - 1, m;
                    while (lo < hi) {
                        m = ((lo + 1) >> 1) + (hi >> 1);  // index in the middle
                        if (m == lo || m == hi)
                            break;  // 5th term not found, proceed with next 4th term
                        if      (tetrahedral[m] < residue) lo = m;
                        else if (tetrahedral[m] > residue) hi = m;
                        else {
                            ++summands[4];  // found a sum with 5 terms
                            duplicates += i == j || j == k || k == l || l == m;
                            if (verbosity == NORMAL)
                                printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetrahedral[i], tetrahedral[j], tetrahedral[k], tetrahedral[l], tetrahedral[m]);
                            break;  // 5th term found, proceed with next 4th term
                        };
                    }
                }
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // Command line options.
    const char *const progname = argv[0];  // save program name
    opterr = 0;  // don't let getopt show error messages
    int ch;
    while ((ch = getopt(argc, argv, "qs")) != -1) {
        switch (ch) {
            case 'q': verbosity = QUIET; break;
            case 's': verbosity = SILENT; break;
            default: usage(progname);  // exit with failure
        }
    }
    // Shift options out.
    argc -= optind;
    argv += optind;

    // Optional command line argument.
    if (argc > 1)
        usage(progname);  // exit with failure
    if (argc == 1) {
        const long long tmp = atoll(argv[0]);
        if (tmp > 0 && tmp <= UINT32_MAX)
            target = (uint32_t)tmp;
        else
            usage(progname);  // exit with failure
    }

    // Tetrahedral root via Cardano's formula for 3rd-degree polynomials.
    const long double r1 = (long double)target * 3;
    const long double r2 = sqrtl(3) / 9;
    const long double r = sqrtl((r1 + r2)) * sqrtl((r1 - r2));
    const long double root = cbrtl(r1 + r) + cbrtl(r1 - r);
    tetracount = 1 + (int)(floorl(root));  // might be 1 too high

    // Make array of tetrahedral numbers to choose from.
    tetrahedral = malloc((size_t)tetracount * sizeof *tetrahedral);
    tetrahedral[0] = 0;  // Te(0)=0
    tetrahedral[1] = 1;  // Te(1)=1
    for (int n = 2; n < tetracount; ++n)
        tetrahedral[n] = tetrahedral[n - 1] - tetrahedral[n - 2] + tetrahedral[n - 1] + (uint32_t)n;  // recursive relation
    while (tetracount > 1 && (tetrahedral[tetracount - 1] > target || tetrahedral[tetracount - 1] < tetrahedral[tetracount - 2]))
        --tetracount;  // adjust for too many, or for overflow

    // Launch parallel threads.
    const int cores = coresavail(1, MAXTHREADS);
    pthread_t tid[MAXTHREADS];  // thread IDs
    int arg[MAXTHREADS];  // thread arguments
    threads = 0;  // number of threads launched
    for (int res = 0; !res && threads < cores; ++threads) {
        arg[threads] = threads + 1;  // thread 0 starts with Te(1), thread 1 with Te(2), etc.
        res = pthread_create(&tid[threads], NULL, loop, &arg[threads]);
    }
    if (threads) {
        // Wait for all threads to finish.
        for (int i = 0; i < threads; ++i)
            pthread_join(tid[i], NULL);
    } else {
        // Run a single thread manually.
        threads = 1;
        arg[0] = 1;
        loop(&arg[0]);
    }

    // Summary
    if (verbosity != SILENT) {
        const int total = summands[0] + summands[1] + summands[2] + summands[3] + summands[4];
        if (summands[0] || summands[1] || summands[2] || summands[3] || (verbosity == NORMAL && total))
            printf("\n");
        printf("Target          : %"PRIu32"\n", target);
        printf("Sums found      : %d\n", total);
        printf("All distinct    : %d\n", total - duplicates);
        printf("With duplicates : %d\n", duplicates);
        printf("Terms 1,2,3,4,5 : ");
    }
    printf("%d,%d,%d,%d,%d\n", summands[0], summands[1], summands[2], summands[3], summands[4]);

    free(tetrahedral);
    return 0;
}

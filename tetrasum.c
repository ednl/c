/******************************************************************************
 * Brady Haran of Numberphile asked: which 5 tetrahedral numbers sum to 343867?
 * This is my multi-threaded solution that finds 322 different combinations of
 * 5 tetrahedral numbers (277 all unique, 45 with duplicates).
 *
 * Compile:
 *   Mac: clang -std=gnu17 -Ofast -march=native -Wall -Wextra -o tetrasum tetrasum.c
 *   Linux: gcc -std=gnu17 -Ofast -march=native -Wall -Wextra -o tetrasum tetrasum.c -lm
 *   No errors found with: cc -g3 -fsanitize=thread,undefined tetrasum.c -lm
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
 *   2024-05-23 v3: With fancy getopt processing, also works on Linux now.
 *   2024-05-24 v4: Simpler without dynamic memory allocation, faster
 *                  without binary search.
 *
 * Extra info:
 * Numberphile video is out now: https://www.youtube.com/watch?v=CK3_zarXkw0
 * Turns out this was in reference to one of Pollock's conjectures, see
 * https://en.wikipedia.org/wiki/Pollock%27s_conjectures and 343867 is thought
 * to be the last (largest) sum that needs five tetrahedral numbers. But they
 * don't have to be distinct! E.g. 17 is the first number that needs more
 * than four: 17 = 1 + 1 + 1 + 4 + 10.
 ******************************************************************************/

#if __APPLE__
    #include <sys/sysctl.h>  // sysctlbyname
#elif __linux__
    #define _GNU_SOURCE  // must come before all includes, not just sched.h
    #include <sched.h>   // sched_getaffinity
#endif
#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // atoll, atoi, exit, EXIT_FAILURE
#include <math.h>      // cbrt, floor
#include <stdint.h>    // uint32_t, UINT32_MAX
#include <inttypes.h>  // PRIu32
#include <unistd.h>    // getopt, opterr, optind
#include <pthread.h>   // pthread_create, pthread_join

// Pollock's conjecture: all positive whole numbers are sums
// of at most 5 tetrahedral numbers (duplicates allowed).
// First number that requires more than 4 terms is 17.
// Last number that requires more than 4 terms is probably 343867.
#define DEFTARGET   343867
#define MAXSUMMANDS 5

// Largest tetrahedral number to fit in uint32_t is Te(2952)
// Te(2952)=4291795704 < 2^32 < Te(2953)=4296157285
#define TETRACOUNT 2953

// Te(4801278)=18446741848412178987 < 2^64 < Te(4801279)=18446753374556197973
// #define TETRACOUNT64 4801279

// Arbitrary limit, avoid dynamic allocation of arg array
#define MAXTHREADS 32

// Cube root of 6
#define CR6 1.8171205928321397

// Large numbers have many tetrahedral sums with 5 terms.
// Use command line option -q (=quiet) to not print sums with 5 terms.
// Use command line option -s (=silent) to not print any sums.
typedef enum verbosity {
    SILENT, QUIET, NORMAL
} Verbosity;

// Data going in and out of a thread.
typedef struct data {
    int start, end, step;   // loop params going in
    int sums[MAXSUMMANDS];  // how many sums per term count
    int duplicates;         // how many sums (any term count) have duplicate terms
} Data;

// Find all tetrahedral sums with up to 5 terms for this number.
static uint32_t target = DEFTARGET;

// Array with room for all 32-bit tetrahedral numbers [0,1,4,10,...,4291795704]
// but only filled at init until Te(N) <= target, with N to be determined.
static uint32_t tetrahedral[TETRACOUNT];

// All globals are read-only after init, so no atomicity needed.
static Verbosity verbosity = NORMAL;

// Help text.
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
    #elif _WIN32
+       n = atoi(getenv("NUMBER_OF_PROCESSORS") ?: "1");
    #endif
    return n < lo ? lo : (n > hi ? hi : n);
}

// Parallel execution in separate threads.
static void *loop(void *arg)
{
    Data *data = arg;

    // Local const copies might be faster?
    const int start = data->start;
    const int end   = data->end;
    const int step  = data->step;

    // First term, each thread starts at a different index and
    // step is thread count so all threads are completely separate.
    for (int i = start; i < end; i += step) {
        const uint32_t partial1 = tetrahedral[i];  // partial sum with 1 term
        if (partial1 == target) {
            data->sums[0]++;  // found a sum with 1 term (i.e. target is tetrahedral)
            if (verbosity != SILENT)
                printf("%"PRIu32"\n", tetrahedral[i]);
        }

        // Second term, duplicates allowed.
        for (int j = i; j < end; ++j) {
            const uint32_t partial2 = partial1 + tetrahedral[j];  // partial sum with 2 terms
            if (partial2 > target || partial2 < partial1)
                break;  // impossible sum or overflow, proceed with next 1st term
            if (partial2 == target) {
                data->sums[1]++;  // found a sum with 2 terms
                data->duplicates += i == j;
                if (verbosity != SILENT)
                    printf("%"PRIu32" + %"PRIu32"\n", tetrahedral[i], tetrahedral[j]);
                break;  // can't have more solutions, proceed with next 1st term
            }

            // Third term, duplicates allowed.
            for (int k = j; k < end; ++k) {
                const uint32_t partial3 = partial2 + tetrahedral[k];  // partial sum with 3 terms
                if (partial3 > target || partial3 < partial2)
                    break;  // impossible sum or overflow, proceed with next 2nd term
                if (partial3 == target) {
                    data->sums[2]++;  // found a sum with 3 terms
                    data->duplicates += i == j || j == k;
                    if (verbosity != SILENT)
                        printf("%"PRIu32" + %"PRIu32" + %"PRIu32"\n",
                            tetrahedral[i], tetrahedral[j], tetrahedral[k]);
                    break;  // can't have more solutions, proceed with next 2nd term
                }

                // Fourth term, duplicates allowed.
                for (int l = k; l < end; ++l) {
                    const uint32_t partial4 = partial3 + tetrahedral[l];  // partial sum with 4 terms
                    if (partial4 > target || partial4 < partial3)
                        break;  // impossible sum or overflow, proceed with next 3rd term
                    if (partial4 == target) {
                        data->sums[3]++;  // found a sum with 4 terms
                        data->duplicates += i == j || j == k || k == l;
                        if (verbosity != SILENT)
                            printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n",
                                tetrahedral[i], tetrahedral[j], tetrahedral[k], tetrahedral[l]);
                        break;  // can't have more solutions, proceed with next 3rd term
                    }

                    // What's left for a potential 5th term?
                    const uint32_t residue = target - partial4;  // target is always greater than partial4

                    // To find a sum with 5 terms, residue must be a tetrahedral number in range.
                    if (residue < tetrahedral[l] || residue > tetrahedral[end - 1])
                        continue;  // 5th term is impossible, proceed with next 4th term

                    // Cube root of residue*6 is m or just below m, where Te(m)=residue
                    int m = (int)(floor(cbrt(residue) * CR6));
                    while (m < end && tetrahedral[m] < residue)
                        ++m;  // might have been 1 or 2 too low, because of approximation and floor()
                    if (m < end && tetrahedral[m] == residue) {
                        data->sums[4]++;  // found a sum with 5 terms
                        data->duplicates += i == j || j == k || k == l || l == m;
                        if (verbosity >= NORMAL)
                            printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n",
                                tetrahedral[i], tetrahedral[j], tetrahedral[k], tetrahedral[l], tetrahedral[m]);
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
    int tetracount = 1 + (int)(floorl(root));  // might be 1 too high
    if (tetracount > TETRACOUNT)
        tetracount = TETRACOUNT;

    // Make array of tetrahedral numbers to choose from.
    tetrahedral[0] = 0;  // Te(0)=0
    tetrahedral[1] = 1;  // Te(1)=1
    for (int n = 2; n < tetracount; ++n)
        tetrahedral[n] = tetrahedral[n - 1] - tetrahedral[n - 2] + tetrahedral[n - 1] + (uint32_t)n;  // recursive relation
    while (tetracount && tetrahedral[tetracount - 1] > target)
        --tetracount;  // adjust for too many

    // Launch parallel threads.
    const int threads = coresavail(1, MAXTHREADS);
    pthread_t tid[MAXTHREADS];  // thread IDs
    Data arg[MAXTHREADS];  // thread arguments going in and out
    int spawned = 0;
    for (int i = 0; i < threads; ++i) {
        arg[i] = (Data){
            .start = i + 1,  // thread 0 starts with Te(1), thread 1 with Te(2), etc.
            .end = tetracount,
            .step = threads
        };  // output vars init to zero
        if (!pthread_create(&tid[i], NULL, loop, &arg[i]))
            ++spawned;
    }

    // Exit if not all threads could be created.
    if (spawned < threads) {
        fprintf(stderr, "Unable to launch %d of %d threads.\n", threads - spawned, threads);
        exit(EXIT_FAILURE);
    }

    // Wait for all threads to finish.
    for (int i = 0; i < threads; ++i)
        pthread_join(tid[i], NULL);

    // Collate data from all threads into first data record arg[0].
    for (int i = 1; i < threads; ++i) {
        for (int j = 0; j < MAXSUMMANDS; ++j)
            arg[0].sums[j] += arg[i].sums[j];
        arg[0].duplicates += arg[i].duplicates;
    }

    // Summary
    if (verbosity != SILENT) {
        int total = 0;
        for (int i = 0; i < MAXSUMMANDS; ++i)
            total += arg[0].sums[i];
        if (total > arg[0].sums[4] || (verbosity >= NORMAL && total))
            printf("\n");
        printf("Target          : %"PRIu32"\n", target);
        printf("Sums found      : %d\n", total);
        printf("All distinct    : %d\n", total - arg[0].duplicates);
        printf("With duplicates : %d\n", arg[0].duplicates);
        printf("Terms 1,2,3,4,5 : ");
    }
    for (int i = 0; i < MAXSUMMANDS; ++i)
        printf("%d%c", arg[0].sums[i], i < MAXSUMMANDS - 1 ? ',' : '\n');

    return 0;
}

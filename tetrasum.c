/******************************************************************************
 * Brady Haran of Numberphile asked: which 5 tetrahedral numbers sum to 343867?
 * This is my multi-threaded solution that finds 322 different combinations of
 * 5 tetrahedral numbers (277 all unique, 45 with duplicates).
 *
 * Compile: clang -std=gnu17 -Ofast -o tetrasum tetrasum.c
 * Run: ./tetrasum 343867     : show sums with up to 4 terms, and summary
 *  or: ./tetrasum -v 343867  : show everything (verbose)
 *  or: ./tetrasum -q 343867  : only show no. of solutions (quiet)
 *
 * E. Dronkert
 * Utrecht, Netherlands
 * https://github.com/ednl/c/blob/master/tetrasum.c
 *
 * 2024-05-20 v1  First try with unique combinations.
 * 2024-05-23 v2  Now as requested with duplicates, and threads.
 *
 * Numberphile video is out now: https://www.youtube.com/watch?v=CK3_zarXkw0
 * Turns out this was in reference to one of Pollock's conjectures, see
 * https://en.wikipedia.org/wiki/Pollock%27s_conjectures and 343867 is thought
 * to be the last (largest) sum that needs five tetrahedral numbers. But they
 * don't have to be distinct! E.g. 17 is the first (smallest) that needs more
 * than four: 17 = 10 + 4 + 1 + 1 + 1.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>    // malloc, free
#include <math.h>      // cbrt, floor
#include <stdint.h>    // uint32_t, UINT32_MAX
#include <inttypes.h>  // PRIu32
#include <string.h>    // strcmp
#if __APPLE__
    #include <sys/sysctl.h>  // sysctlbyname
    #include <pthread.h>     // pthread_create, pthread_join
#elif __linux__
    #define _GNU_SOURCE
    #include <sched.h>  // sched_getaffinity
    // TODO: use threads on Linux
#endif

#define MAXTHREADS 32
static uint32_t threads, target, N, total, dup, part[5];
static uint32_t *tetra;
static uint8_t verbose = 1;

// How many CPU cores available to this program?
static int coresavail(const int min, const int max)
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
    return n < min ? min : (n > max ? max : n);
}

// Parallel execution in separate threads.
static void *loop(void *arg)
{
    const uint32_t start = *(uint32_t *)arg;

    for (uint32_t i = start; i < N; i += threads) {
        uint32_t a = tetra[i];
        if (a == target) {
            ++total;
            ++part[0];
            if (verbose)
                printf("%"PRIu32"\n", tetra[i]);
        }

        for (uint32_t j = i; j < N; ++j) {
            uint32_t b = a + tetra[j];
            if (b < a || b > target)
                break;
            if (b == target) {
                ++total;
                ++part[1];
                dup += i == j;
                if (verbose)
                    printf("%"PRIu32" + %"PRIu32"\n", tetra[i], tetra[j]);
                break;
            }

            for (uint32_t k = j; k < N; ++k) {
                uint32_t c = b + tetra[k];
                if (c < b || c > target)
                    break;
                if (c == target) {
                    ++total;
                    ++part[2];
                    dup += i == j || j == k;
                    if (verbose)
                        printf("%"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetra[i], tetra[j], tetra[k]);
                    break;
                }

                for (uint32_t l = k; l < N; ++l) {
                    uint32_t d = c + tetra[l];
                    if (d < c || d > target)
                        break;
                    if (d == target) {
                        ++total;
                        ++part[3];
                        dup += i == j || j == k || k == l;
                        if (verbose)
                            printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetra[i], tetra[j], tetra[k], tetra[l]);
                        break;
                    }

                    // Try to find fifth term.
                    uint32_t res = target - d;  // residue
                    if (res < tetra[l] || res > tetra[N - 1])
                        continue;
                    if (res == tetra[l] || res == tetra[N - 1]) {
                        uint32_t m = res == tetra[l] ? l : N - 1;
                        ++total;
                        ++part[4];
                        dup += i == j || j == k || k == l || l == m;
                        if (verbose == 2)
                            printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetra[i], tetra[j], tetra[k], tetra[l], tetra[m]);
                        continue;
                    }

                    // No edge case, now do binary search.
                    uint32_t min = l, max = N - 1, m;
                    while (min < max) {
                        m = ((min + 1) >> 1) + (max >> 1);
                        if (m == min || m == max) break;
                        if      (tetra[m] < res) min = m;
                        else if (tetra[m] > res) max = m;
                        else {
                            ++total;
                            ++part[4];
                            dup += i == j || j == k || k == l || l == m;
                            if (verbose == 2)
                                printf("%"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32" + %"PRIu32"\n", tetra[i], tetra[j], tetra[k], tetra[l], tetra[m]);
                            break;
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
    const int cores = coresavail(1, MAXTHREADS);
    pthread_t tid[MAXTHREADS];
    uint32_t arg[MAXTHREADS];

    // Pollock's conjecture: all numbers are sums of at
    // most 5 tetrahedral numbers (duplicates allowed).
    // This is probably the largest number that requires 5.
    target = 343867;

    // Command line arguments.
    if (argc > 1) {
        int argofs = 0;
        // Verbosity level
        if (!strcmp(argv[1], "-q") || !strcmp(argv[1], "--quiet")) {
            verbose = 0;
            argofs = 1;
        } else if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--verbose")) {
            verbose = 2;
            argofs = 1;
        }
        // Number under investigation
        if (argc - argofs > 1) {
            const long long a = atoll(argv[argofs + 1]);
            if (a > 0 && a <= UINT32_MAX)
                target = (uint32_t)a;
        }
    }

    // Tetrahedral root with Cardano's formula for 3rd-degree polynomials.
    const long double r1 = (long double)target * 3, r2 = sqrtl(3) / 9, r = sqrtl((r1 + r2)) * sqrtl((r1 - r2)), ca = cbrtl(r1 + r) + cbrtl(r1 - r);
    N = 1 + (uint32_t)(floorl(ca));  // might be 1 too high

    // Precompute the first N tetrahedral numbers to choose from.
    tetra = malloc(N * sizeof *tetra);
    tetra[0] = 0;  // Te(0)=0
    tetra[1] = 1;  // Te(1)=1
    for (uint32_t n = 2; n < N; ++n)
        tetra[n] = tetra[n - 1] - tetra[n - 2] + tetra[n - 1] + n;  // recursive relation overflows later
    while (N > 1 && (tetra[N - 1] > target || tetra[N - 1] < tetra[N - 2]))
        --N;  // adjust for too many, or overflow

    // Launch parallel threads.
    threads = 0;
#if __APPLE__
    for (int i = 0, res = 0; !res && i < cores; ++i, ++threads) {
        arg[i] = (uint32_t)(i + 1);
        res = pthread_create(&tid[i], NULL, loop, &arg[i]);
    }
    for (uint32_t i = 0; i < threads; ++i)
        pthread_join(tid[i], NULL);
#elif __linux__
    // TODO: use threads on Linux
#endif

    // Single-threaded backup if no threads.
    if (!threads) {
        threads = 1;
        arg[0] = 1;
        loop(&arg[0]);
    }

    // Summary
    if (verbose) {
        if ((verbose == 2 && total) || part[0] + part[1] + part[2] + part[3])
            printf("\n");
        printf("Target          : %"PRIu32"\n", target);
        printf("Sums found      : %"PRIu32"\n", total);
        printf("All distinct    : %"PRIu32"\n", total - dup);
        printf("With duplicates : %"PRIu32"\n", dup);
        printf("Terms 1,2,3,4,5 : ");
    }
    printf("%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"\n", part[0], part[1], part[2], part[3], part[4]);

    free(tetra);
    return 0;
}

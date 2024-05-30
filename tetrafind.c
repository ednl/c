/******************************************************************************
 * Pollock conjectured that all positive whole numbers can be written as a sum
 * of at most five tetrahedral numbers (duplicates allowed). The first number
 * that requires at least five terms is 17 (= 1+1+1+4+10), the last one is
 * "almost certainly" 343867 (multiple different sums exist).
 *
 * This program searches for 64-bit integers that require more than four terms
 * in their tetrahedral sums. It starts at the number specified on the command
 * line, or else at 1. Stop the search by pressing Ctrl-C. The last number
 * checked will be reported.
 *
 * Compile:
 *   Mac: clang -std=gnu17 -Ofast -march=native -o tetrafind tetrafind.c
 *   Linux: gcc -std=gnu17 -Ofast -march=native -o tetrafind tetrafind.c -lm
 * No warnings or errors found with:
 *   \clang -g3 -fsanitize=thread,undefined -Wall -Wextra tetrafind.c
 *
 * Made by:
 *   E. Dronkert
 *   Utrecht, Netherlands
 *   https://github.com/ednl/c/blob/master/tetrafind.c
 ******************************************************************************/

// Platform-specific libraries
#if __APPLE__
    #include <sys/sysctl.h>  // sysctlbyname
#elif __linux__
    #define _GNU_SOURCE      // must come before all includes, not just sched.h
    #include <sched.h>       // sched_getaffinity
#endif

// Standard libraries
#include <stdio.h>      // printf, fprintf, stderr
#include <stdlib.h>     // strtoull, atoi, getenv
#include <errno.h>      // errno, ERANGE
#include <math.h>       // cbrtl, floorl
#include <stdint.h>     // uint64_t, UINT64_MAX, UINT64_C
#include <inttypes.h>   // PRIu64
#include <stdbool.h>    // bool, true, false
#include <stdatomic.h>  // atomic_bool, atomic_load_explicit, atomic_store_explicit
#include <signal.h>     // signal, sig_atomic_t

// POSIX library, probably available on all three of Win/Mac/Linux
#include <pthread.h>    // pthread_create, pthread_join

// Pollock's conjecture: all positive whole numbers are sums
// of at most 5 tetrahedral numbers (duplicates allowed).
// First number that requires more than 4 terms is 17.
// Last number that requires more than 4 terms is believed to be 343867.
#define FIRSTTARGET 1

// Largest tetrahedral number to fit in uint64_t is Te(4801278)
// Te(4801278)=18446738006366306560 < 2^64 <= Te(4801279)=18446749532508725120
#define TE_SIZE 4801279

// Arbitrary limit to avoid dynamic allocation of arg array
#define MAXTHREADS 32

// Cube root of 6 (long double for LDBL_DECIMAL_DIG=21)
#define LDBL_CR6 1.817120592832139658909L

// Thread arguments
typedef struct threaddata {
    atomic_bool *solutionfound;
    uint64_t target;
    int begin, end, step;  // outer loop parameters
} ThreadData;

// Array with room for all 64-bit tetrahedral numbers [0,1,4,10,...,18446738006366306560]
// but only filled at init for all Te(N) <= target, with N to be determined per target.
static uint64_t Te[TE_SIZE];

// Catch SIGINT for a clean exit with progress report.
static volatile sig_atomic_t aborted;

// Number of CPU cores available to the program.
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
        // Patch by /u/skeeto.
        n = atoi(getenv("NUMBER_OF_PROCESSORS") ?: "1");
    #endif
    return n < lo ? lo : (n > hi ? hi : n);
}

// Next tetrahedral number at index n via recursive relation to avoid overflow.
static inline void settetra(const int n)
{
    Te[n] = Te[n - 1] - Te[n - 2] + Te[n - 1] + (uint64_t)n;
}

// Find tetrahedral index of x if x is tetrahedral, otherwise -1.
static inline int tetraindex(const uint64_t x)
{
    const int n = (int)(floorl(cbrtl(x) * LDBL_CR6));  // 0 <= n <= 4801279
    return n < TE_SIZE && Te[n] == x ? n : -1;
}

// Parallel execution in separate threads.
static void *loop(void *arg)
{
    const ThreadData *const dat = arg;
    atomic_bool *const found = dat->solutionfound;
    const uint64_t target = dat->target;
    const int end = dat->end;
    const int step = dat->step;

    for (int i = dat->begin; i < end; i += step) {
        // Never a solution because of invariant: Te[N-1] < target.
        const uint64_t partial1 = Te[i];  // partial sum with 1 term

        for (int j = i; j < end; ++j) {
            const uint64_t partial2 = partial1 + Te[j];  // partial sum with 2 terms
            if (partial2 > target || partial2 < partial1) {
                // Partial sum with 2 terms is too large or overflows.
                if (j > i)
                    break;  // continue with next 1st term i
                else
                    return NULL;  // impossible to find solutions from here
            }
            if (partial2 == target) {
                atomic_store_explicit(found, true, memory_order_relaxed);
                return NULL;  // found tetrahedral sum with 2 terms
            }

            for (int k = j; k < end; ++k) {
                // Check if solution found in other thread, or interrupted by Ctrl-C
                if (atomic_load_explicit(found, memory_order_relaxed) || aborted)
                    return NULL;  // stop this thread

                const uint64_t partial3 = partial2 + Te[k];  // partial sum with 3 terms
                if (partial3 > target || partial3 < partial2)
                    // Partial sum with 3 terms is too large or overflows.
                    break;  // continue with next 2nd term j
                if (partial3 == target) {
                    atomic_store_explicit(found, true, memory_order_relaxed);
                    return NULL;  // found tetrahedral sum with 3 terms
                }

                const uint64_t residue = target - partial3;
                if (residue < Te[k] || residue > Te[end - 1])
                    continue;  // 4th term is impossible, proceed with next 3rd term
                if (tetraindex(residue) != -1) {
                    atomic_store_explicit(found, true, memory_order_relaxed);
                    return NULL;  // found tetrahedral sum with 4 terms
                }
            }
        }
    }
    return NULL;
}

// Signal handler for SIGINT (Ctrl-C)
static void sighandler(int sig)
{
    aborted = sig;
}

int main(int argc, char *argv[])
{
    // Use as many threads as cores available.
    const int threadcount = coresavail(1, MAXTHREADS);

    uint64_t target = FIRSTTARGET;
    if (argc > 1) {
        char *end;
        const uint64_t a = strtoull(argv[1], &end, 0);
        if (errno == ERANGE || *end != '\0' || !a) {  // target can't be zero
            errno = 0;
            fprintf(stderr, "Argument must be a positive whole number smaller than 2^64.\n");
            exit(EXIT_FAILURE);
        }
        target = a;
    }
    printf("Searching for positive whole numbers that have tetrahedral sums\n"
        "with more than four terms, starting from %"PRIu64":\n", target);

    // Initial array of N tetrahedral numbers to choose from.
    Te[0] = 0;
    Te[1] = 1;
    int N = 2;  // length currently used of the Te[] array
    for (; N < TE_SIZE && Te[N - 1] < target; ++N)
        settetra(N);
    if (Te[N - 1] >= target)
        --N;
    else if (N < TE_SIZE && !Te[N])
        settetra(N);
    // Invariant now true: N >= 1, Te[N-1] < target and Te[N] already set.

    // Count numbers whose tetrahedral sum requires more than 4 terms. From 17 to
    // 343867, that's 241 numbers. Compare: https://oeis.org/A000797/b000797.txt
    int require5 = 0;

    // Catch Ctrl-C interrupt to report last target checked.
    signal(SIGINT, sighandler);

    for (; target && !aborted; ++target) {  // probably superfluous check for Ctrl-C, see below
        // Check invariant and fix when necessary.
        if (N < TE_SIZE && Te[N] < target)
            if (++N < TE_SIZE)
                settetra(N);

        // Is target tetrahedral? If so, can only be Te[N]. Threads won't catch
        // this because Te[N-1]<target, so must check here. Also check if target
        // is close to Te[N-1] to avoid much work when it would have solutions
        // with 2-4 terms anyway:
        //   x=1+Te, x=1+1+Te, x=1+1+1+Te, x=4+Te, x=1+4+Te, x=1+1+4+Te
        if (target - Te[N - 1] <= 6 || (N < TE_SIZE && target == Te[N]))
            continue;  // uninteresting

        // Launch parallel threads to search for any tetrahedral sum with up to 4 terms.
        atomic_bool solutionfound = false;  // set flag to true in any thread to stop all threads
        pthread_t tid[MAXTHREADS];   // thread IDs
        ThreadData arg[MAXTHREADS];  // thread arguments
        for (int i = 0; i < threadcount; ++i) {
            arg[i] = (ThreadData){ &solutionfound, target, i + 1, N, threadcount };
            if (pthread_create(&tid[i], NULL, loop, &arg[i])) {
                fprintf(stderr, "Unable to launch thread %d of %d.\n", i + 1, threadcount);
                exit(EXIT_FAILURE);
            }
        }

        // Wait for all threads to finish. Thread that finds solution first
        // sets 'solutionfound', which will end all other threads.
        for (int i = 0; i < threadcount; ++i)
            pthread_join(tid[i], NULL);

        // Most likely interrupted while busy in threads, so check after joining
        // and don't go to next target before reporting.
        if (aborted)
            break;

        // Interesting: no solution means more than 4 terms required for tetrahedral sum.
        if (!solutionfound)
            printf("%d %"PRIu64"\n", ++require5, target);
    }

    // Show starting point for next run.
    if (aborted)
        printf("\nInterrupted while checking %"PRIu64"\n", target);

    return 0;
}

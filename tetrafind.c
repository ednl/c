/******************************************************************************
 * Pollock conjectured that all positive whole numbers can be written as a sum
 * of at most 5 tetrahedral numbers (duplicates allowed). The first number that
 * requires at least 5 terms is 17 (1+1+1+4+10), the last one is believed to be
 * 343867 which has 322 unique tetrahedral sums of 5 terms (summands).
 *
 * This program finds 64-bit integers that require more than four terms in
 * their tetrahedral sums. It starts at the number specified on the command
 * line, or else at 0. Stop the search by pressing Ctrl-C. The last number
 * checked will be printed.
 *
 * Compile:
 *   Mac: clang -std=gnu17 -Ofast -march=native -Wall -Wextra -o tetrafind tetrafind.c
 *   Linux: gcc -std=gnu17 -Ofast -march=native -Wall -Wextra -o tetrafind tetrafind.c -lm
 *   No errors found with: cc -g3 -fsanitize=thread,undefined tetrafind.c -lm
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
#include <stdatomic.h>  // atomic_bool
#include <signal.h>     // signal, sig_atomic_t

// POSIX library, probably available on all three of Win/Mac/Linux.
#include <pthread.h>    // pthread_create, pthread_join

// Pollock's conjecture: all positive whole numbers are sums
// of at most 5 tetrahedral numbers (duplicates allowed).
// First number that requires more than 4 terms is 17.
// Last number that requires more than 4 terms is believed to be 343867.
#define FIRSTTARGET 0
#define MAXSUMMANDS 4

// Largest tetrahedral number to fit in uint64_t is Te(4801278)
// Te(4801278)=18446738006366306560 < 2^64 <= Te(4801279)=18446749532508725120
#define TE_SIZE 4801279

// Arbitrary limit to avoid dynamic allocation of arg array
#define MAXTHREADS 32

// Cube root of 6 (long double for LDBL_DECIMAL_DIG=21)
#define LDBL_CR6 1.817120592832139658909L

// Data going in and out of a thread.
typedef struct threaddata {
    int threadnum;           // thread number [1..threadcount]
    int index[MAXSUMMANDS];  // result: indices of Te numbers that sum to target
} ThreadData;

// Find first combination of at most 4 tetrahedral numbers that sum to target,
// starting with FIRSTTARGET.
static uint64_t target = FIRSTTARGET;

// Number of threads is also step size of outermost loop in each thread.
static int threadcount;

// Array with room for all 64-bit tetrahedral numbers [0,1,4,10,...,18446738006366306560]
// but only filled at init for all Te(N) <= target, with N to be determined per target.
static uint64_t Te[TE_SIZE];

// N is number of elements used of Te[] array, range [0..TE_SIZE].
static int N;

// Inter-thread communication:
//  - is reset by 1 thread on first solution found
//  - regularly checked by all threads
//  - return from thread if no longer set
static atomic_bool running;

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

// Tetrahedral root approximation of x. Suggestion by /u/purplefunctor.
// Return: index n (0 <= n <= 4801279) where Te(n)=x only if x is tetrahedral, n+/-1 otherwise.
static inline int tetraroot(const uint64_t x)
{
    return (int)(floorl(cbrtl(x) * LDBL_CR6));
}

// Find tetrahedral index of x if x is tetrahedral, otherwise -1.
static inline int tetraindex(const uint64_t x)
{
    const int n = tetraroot(x);
    return n < TE_SIZE && Te[n] == x ? n : -1;
}

// Parallel execution in separate threads.
static void *loop(void *arg)
{
    ThreadData *localdata = arg;
    const int start = localdata->threadnum;  // thread number [1..threadcount]

    // Maybe local const copies are faster than accessing shared (but read-only) globals?
    const int end = N;  // current length of Te[] array
    const int step = threadcount;  // from coresavail() at init

    for (int i0 = start; i0 < end; i0 += step) {
        const uint64_t partial0 = Te[i0];  // partial sum with 1 term
        if (partial0 > target)
            break;
        if (partial0 == target && running && !aborted) {
            running = false;
            localdata->index[0] = i0;
            return NULL;
        }

        for (int i1 = i0; i1 < end; ++i1) {
            const uint64_t partial1 = partial0 + Te[i1];  // partial sum with 2 terms
            if (partial1 > target || partial1 < partial0)
                break;
            if (partial1 == target && running && !aborted) {
                running = false;
                localdata->index[0] = i0;
                localdata->index[1] = i1;
                return NULL;
            }

            for (int i2 = i1; i2 < end; ++i2) {
                if (!running || aborted)
                    return NULL;
                const uint64_t partial2 = partial1 + Te[i2];  // partial sum with 2 terms
                if (partial2 > target || partial2 < partial1)
                    break;
                if (partial2 == target && running && !aborted) {
                    running = false;
                    localdata->index[0] = i0;
                    localdata->index[1] = i1;
                    localdata->index[2] = i2;
                    return NULL;
                }

                const uint64_t residue = target - partial2;
                if (residue < Te[i2] || residue > Te[end - 1])
                    continue;  // 4th term is impossible, proceed with next 3rd term
                const int i3 = tetraindex(residue);
                if (i3 != -1 && running && !aborted) {
                    running = false;
                    localdata->index[0] = i0;
                    localdata->index[1] = i1;
                    localdata->index[2] = i2;
                    localdata->index[3] = i3;
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

// Next tetrahedral number Te(n) via recursive relation to avoid overflow.
static inline void settetra(const int n)
{
    Te[n] = Te[n - 1] - Te[n - 2] + Te[n - 1] + (uint64_t)n;
}

// Signal handler for SIGINT (Ctrl-C)
static void sighandler(int sig)
{
    aborted = sig;
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        char *end;
        const uint64_t a = strtoull(argv[1], &end, 0);
        if (errno == ERANGE || *end != '\0') {
            errno = 0;
            fprintf(stderr, "Invalid argument, must be a whole number N where 0 <= N < 2^64.\n");
            exit(EXIT_FAILURE);
        }
        target = a;
    }

    // Initial array of N tetrahedral numbers to choose from.
    Te[0] = 0;
    Te[1] = 1;
    for (N = 2; N < TE_SIZE && Te[N - 1] < target; ++N)
        settetra(N);
    if (Te[N - 1] > target)
        --N;  // now true: last number Te[N-1] <= target

    // Use as many threads as cores available.
    threadcount = coresavail(1, MAXTHREADS);

    // Catch Ctrl-C interrupt to report last target checked.
    signal(SIGINT, sighandler);

    for (; !aborted; ++target) {
        // Add tetrahedral number when necessary
        if (N < TE_SIZE) {
            if (!Te[N])
                settetra(N);
            if (Te[N] <= target)
                ++N;
        }

        // Is target itself tetrahedral?
        if (target == Te[N - 1])
            continue;  // not interesting

        // Launch parallel threads.
        pthread_t tid[MAXTHREADS];   // thread IDs
        ThreadData arg[MAXTHREADS];  // thread arguments going in and out
        running = true;
        for (int i = 0; i < threadcount; ++i) {
            arg[i] = (ThreadData){.threadnum = i + 1};  // output var .index[] inits to zeros
            if (pthread_create(&tid[i], NULL, loop, &arg[i])) {
                fprintf(stderr, "Unable to launch thread %d of %d.\n", i + 1, threadcount);
                exit(EXIT_FAILURE);
            }
        }

        // Wait for all threads to finish. Thread that finds a solution first,
        // resets 'running', which will end the other threads.
        for (int i = 0; i < threadcount; ++i)
            pthread_join(tid[i], NULL);

        // Result from one thread.
        bool resultfound = false;
        for (int i = 0; i < threadcount; ++i)
            if (arg[i].index[0] > 0) {
                resultfound = true;
                break;
            }

        // Noteworthy: no result means more than 4 terms required for tetrahedral sum.
        if (!resultfound && !aborted)
            printf("%"PRIu64"\n", target);
    }

    if (aborted)
        printf("\nInterrupted while checking %"PRIu64"\n", target);

    return 0;
}

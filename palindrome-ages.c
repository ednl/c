/**
 * Numberphile: Palindrome Ages
 * https://www.youtube.com/watch?v=fwLsCgibGw4
 * https://www.youtube.com/watch?v=bIbTfdbQaSE
 * with Kat Philips and Brady Haran
 *
 * Example from the first video: Kat's mother was 62 when
 * Kat was 26. The two combined are a palindrome: 6226.
 *
 * Compile:
 *     cc -std=gnu17 -Wall -Wextra -pedantic -O3 -march=native -mtune=native palindrome-ages.c
 * Select one specific gap from output:
 *     ./a.out | grep -E '^\s*27' | sort
 * Select one specific base from output:
 *     ./a.out | grep -F '(16)' | sort
 * Select one specific palindrome:
 *     ./a.out | grep poop
 *
 * Code by E. Dronkert, Utrecht, NL
 * https://github.com/ednl/c/blob/main/palindrome-ages.c
 * https://mu.social/profile/ee1.nl
 * https://mastodon.social/@ednl
 * https://ee1.nl
 */

#include <stdio.h>
#include <string.h>  // memset

// Adjust to taste
#define DIGITS  32  // e.g. needs to be >=10 for maxnum=1023,base=2
#define MINBASE 2
#define MAXBASE 36  // 10 numerical digits + 26 letters = 36 symbols
#define MINGAP  9
#define MAXGAP  99
#define MAXNUM  999

static int num[DIGITS];  // number
static int mun[DIGITS];  // reverse

// Store value in digit array
// return: number of digits (0 if overflow)
static int set(int *a, const int base, int x)
{
    memset(a, 0, DIGITS * sizeof *a);
    int k = 0;
    do {
        a[k++] = x % base;
        x /= base;
    } while (x > 0 && k < DIGITS);
    return x ? 0 : k;
}

// Retrieve value from digit array
// k must be 1..DIGITS
static int get(const int *a, const int base, const int k)
{
    int x = a[k - 1];
    for (int i = k - 1; i > 0; )
        x = x * base + a[--i];
    return x;
}

// Reverse digits from array src to array dst
// k: number of digits
static void rev(int *restrict dst, const int *restrict src, const int k)
{
    for (int i = 0, j = k - 1; i < k; )
        dst[i++] = src[j--];
}

// Increase number stored in array
// Make sure that reverse is always smaller or equal
static int inc(int *a, const int base, const int k)
{
    int i = 0, j = k - 1;
    while (i < DIGITS && (++a[i] == base || (i < j && a[i] > a[j]))) {
        a[i++] = 0;
        j--;
    }
    if (i < k) return k;
    if (i < DIGITS) return i + 1;
    return DIGITS;
}

// Print number from array
static void show(const int *a, const int k)
{
    static const char *digit = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = k; i > 0; )
        putchar(digit[a[--i]]);
}

int main(void)
{
    fprintf(stderr, "Gap Age1 Age2 (Base) Palindrome\n");
    fprintf(stderr, "--- ---- ---- ------ --------------------\n");
    for (int base = MINBASE; base <= MAXBASE; ++base) {
        int n = base;
        int k = set(num, base, n);
        while (n <= MAXNUM) {
            rev(mun, num, k);
            const int m = get(mun, base, k);
            const int gap = n - m;
            if (gap >= MINGAP && gap <= MAXGAP) {
                printf("%3d %4d %4d   (%2d) ", gap, n, m, base);
                show(num, k);
                show(mun, k);
                putchar('\n');
            }
            k = inc(num, base, k);
            n = get(num, base, k);
        }
    }
}

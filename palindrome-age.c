#include <stdio.h>
#include <string.h>

#define DIGITS  32
#define MINBASE 2
#define MAXBASE 36
#define MINGAP  16
#define MAXGAP  99
#define MAXNUM  999

static int num[DIGITS];  // number
static int mun[DIGITS];  // reverse

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

// k must be 1..DIGITS
static int get(const int *a, const int base, const int k)
{
    int x = a[k - 1];
    for (int i = k - 1; i > 0; )
        x = x * base + a[--i];
    return x;
}

static void rev(int *restrict dst, const int *restrict src, const int k)
{
    for (int i = 0, j = k - 1; i < k; )
        dst[i++] = src[j--];
}

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

static void show(const int *a, const int k)
{
    static const char *digit = "0123456789abcdefghijklmnopqrstuvwxyz";
    for (int i = k; i > 0; )
        putchar(digit[a[--i]]);
}

int main(void)
{
    for (int base = MINBASE; base <= MAXBASE; ++base) {
        int n = base;
        int k = set(num, base, n);
        while (n <= MAXNUM) {
            rev(mun, num, k);
            const int m = get(mun, base, k);
            const int gap = n - m;
            if (gap >= MINGAP && gap <= MAXGAP) {
                printf("%2d %3d %3d (%2d) ", gap, n, m, base);
                show(num, k);
                show(mun, k);
                putchar('\n');
            }
            k = inc(num, base, k);
            n = get(num, base, k);
        }
    }
}

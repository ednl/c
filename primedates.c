/******************************************************************************
 * Show all prime dates in a year where the date is formatted as YYYYMMDD.
 * Use the current year or the one provided as a command line parameter.
 *
 * Tested on MacOS 11.4 and Raspberry Pi OS 5.10.17 (32-bit).
 *
 * Version : 1.0
 * Date    : 2021-05-25
 * Licence : MIT (free to use as you like, attribution appreciated)
 *
 * (c) E. Dronkert 2021 https://github.com/ednl
 ******************************************************************************/

#include <stdio.h>    // printf
#include <stdlib.h>   // atol, NULL
#include <stdbool.h>  // bool
#include <time.h>     // time, mktime, strftime
#include <locale.h>   // setlocale_r

static bool isprime(int n)
{
    if (n < 2) {
        return false;
    }
    if (n < 4) {
        return true;
    }
    if ((n & 1) == 0) {
        return false;
    }
    // sq0 = f0^2, sq1 = f1^2 = (f0+2)^2 = f0^2 + 4f0 + 4 = sq0 + 4(f0+1)
    int f = 3, sq = 9;
    while (sq <= n) {
        if (n % f++ == 0) {
            return false;
        }
        sq += f++ << 2;
    }
    return true;
}

int main(int argc, char *argv[])
{
    setlocale(LC_TIME, "");
    char buf[32] = {0};
    int n, count = 0, y = 0;
    struct tm T;

    // Use command line parameter or current year
    if (argc > 1 && argv && argv[1]) {
        long a = atol(argv[1]);
        if (a >= 1000 && a <= 9999) {
            y = (int)a;
        }
    }
    if (y == 0) {
        time_t t = time(NULL);
        localtime_r(&t, &T);
        y = T.tm_year + 1900;
    }

    // First date to check = 1 Jan <year>
    T = (struct tm){
        .tm_year = y - 1900,
        .tm_mon  = 0,
        .tm_mday = 1,
    };
    mktime(&T);

    // Check all days of the specified year
    while (T.tm_year + 1900 == y) {
        // n = YYYYMMDD
        n = (T.tm_year + 1900) * 10000 + (T.tm_mon + 1) * 100 + T.tm_mday;
        if (isprime(n)) {
            strftime(buf, sizeof buf, "%a %e %b", &T);
            printf("%2i. %8i = %s\n", ++count, n, buf);
        }
        // Next day
        T.tm_mday++;
        mktime(&T);
    }
    return 0;
}

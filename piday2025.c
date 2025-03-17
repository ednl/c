/**
 * https://ivanr3d.com/projects/pi/2025.html
 * https://www.reddit.com/r/adventofcode/comments/1jb2eis/pi_coding_quest_2025/
 */

#include <stdio.h>   // printf, sprintf, sscanf, FILE, fopen, fclose
#include <string.h>  // strstr
#include <math.h>    // round
#include <stdint.h>  // int64_t
#include <stdbool.h>

// Data file specification
#define FNAME "piday2025.txt"
#define MAXDAYS 30
#define LINELEN 64
#define FIELD1   0
#define FIELD2  13
#define FIELD3  32

// Pi with 32 digits (= 31 decimals)
static const char *pi32 = "31415926535897932384626433832795";

typedef struct stock {
    int day, key;
    double price;
    char digits[8];
    char ticker[8];
    bool manip;
} Stock;

static Stock stock[MAXDAYS];

static int parse(const char *fname)
{
    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;
    char line[LINELEN];
    fgets(line, sizeof line, f);  // discard first line (header)
    int n = 0;
    for (; n < MAXDAYS && fgets(line, sizeof line, f); ++n) {
        sscanf(&line[FIELD1], "%d" , &stock[n].day);
        sscanf(&line[FIELD2], "%lf", &stock[n].price);
        sscanf(&line[FIELD3], "%s" , stock[n].ticker);    // max len: 4+1=5
        stock[n].key = (int)round(stock[n].price * 100);  // max digits: 3+2=5
        sprintf(stock[n].digits, "%d", stock[n].key);     // max len: 5+1=6
    }
    fclose(f);
    return n;  // number of records parsed (= days)
}

static int64_t secret(const int days)
{
    if (days <= 0)
        return 0;

    // Find first manipulated record
    int i = 0;
    while (i < days && !strstr(pi32, stock[i].digits))
        ++i;
    if (i == days)
        return 0;
    stock[i].manip = true;
    double fval = stock[i].price;

    // Find other manipulated records
    for (++i; i < days; ++i)
        if (strstr(pi32, stock[i].digits)) {
            stock[i].manip = true;
            switch (stock[i].day & 1) {
                case 0: fval *= stock[i].price; break;
                case 1: fval /= stock[i].price; break;
            }
        }

    // First 10 digits of floating point value
    int64_t ival = 0;
    char buf[32];
    sprintf(buf, "%.10e", fval);  // 1 unit + 10 decimals = 11 digits total, so no rounding of 10th digit
    for (int i = 0, count = 0; buf[i] && count < 10; ++i)
        if ((buf[i] >= '1' && buf[i] <= '9') || (buf[i] == '0' && count)) {
            ival = ival * 10 + (buf[i] & 15);
            ++count;
        }
    return ival;
}

static char rot(const char c, const int key)
{
    return 'A' + (c - 'A' + key) % ('Z' - 'A' + 1);
}

int main(void)
{
    int days = parse(FNAME);
    printf("Part 1: %lld\n", secret(days));  // 6361428769
    return 0;
}

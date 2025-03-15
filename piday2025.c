#include <stdio.h>
#include <string.h>  // strstr
#include <math.h>    // round
#include <stdint.h>  // int64_t
#include <stdbool.h>

#define FNAME "piday2025.txt"
#define MAXDAYS   32
#define LINELEN   64
#define COLDAY     0
#define COLPRICE  13
#define COLTICKER 32

static const char *pi32 = "31415926535897932384626433832795";

typedef struct stock {
    int day, key;
    double price;
    char digits[8];
    char ticker[8];
    bool manip;
} Stock;

static Stock stock[MAXDAYS];

static int parse(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 0;
    char line[LINELEN];
    fgets(line, sizeof line, f);  // discard first line (header)
    int n = 0;
    for (; n < MAXDAYS && fgets(line, sizeof line, f); ++n) {
        sscanf(&line[COLDAY], "%d", &stock[n].day);
        sscanf(&line[COLPRICE], "%lf", &stock[n].price);
        stock[n].key = (int)round(stock[n].price * 100);
        sprintf(stock[n].digits, "%d", stock[n].key);
        sscanf(&line[COLTICKER], "%4s", stock[n].ticker);
    }
    fclose(f);
    return n;
}

static double hiddenval(const int days)
{
    if (days <= 0)
        return 0;
    int i = 0;
    while (i < days && !strstr(pi32, stock[i].digits))
        ++i;
    if (i == days)
        return 0;
    double secret = stock[i++].price;
    for (; i < days; ++i)
        if (strstr(pi32, stock[i].digits)) {
            stock[i].manip = true;
            switch (stock[i].day & 1) {
                case 0: secret *= stock[i].price; break;
                case 1: secret /= stock[i].price; break;
            }
        }
    return secret;
}

static char rot(const char c, const int key)
{
    return 'A' + (c - 'A' + key) % ('Z' - 'A' + 1);
}

int main(void)
{
    char buf[32];
    sprintf(buf, "%.10e", hiddenval());  // 11 digits total, so no rounding of 10th digit
    int64_t secret = 0;
    for (int i = 0, count = 0; buf[i] && count < 10; ++i)
        if (buf[i] >= '0' && buf[i] <= '9') {
            secret = secret * 10 + (buf[i] & 15);
            ++count;
        }
    printf("Part 1: %lld\n", buf, secret);  // 6361428769

    return 0;
}

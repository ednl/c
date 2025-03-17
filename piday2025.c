/**
 * https://ivanr3d.com/projects/pi/2025.html
 * https://www.reddit.com/r/adventofcode/comments/1jb2eis/pi_coding_quest_2025/
 */

#include <stdio.h>   // printf, sprintf, sscanf, FILE, fopen, fclose
#include <stdlib.h>  // qsort
#include <string.h>  // strstr
#include <math.h>    // round
#include <stdint.h>  // int64_t
#include <stdbool.h>

#define ALPHLEN ('Z' - 'A' + 1)

// Data file specification
#define FNAME "piday2025.txt"
#define MAXDAYS 30
#define LINELEN 64
#define FIELD1   0  // column index of fixed width data
#define FIELD2  13
#define FIELD3  32

// Pi with 32 digits (= 31 decimals)
static const char *pi32 = "31415926535897932384626433832795";

typedef struct stock {
    int day, key;
    double price;
    char digits[8];
    char ticker[8];
} Stock;

static Stock stock[MAXDAYS];
static Stock *manip[MAXDAYS];  // pointers to manipulated stock
static Stock *clean[MAXDAYS];  // pointers to clean stock
static Stock *phrase[MAXDAYS];
static int maniplen, cleanlen, phraselen;

static int parse(const char *fname)
{
    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;
    char line[LINELEN];
    fgets(line, sizeof line, f);  // discard first line (header)
    int n = 0;  // number of records parsed (= days)
    for (; n < MAXDAYS && fgets(line, sizeof line, f); ++n) {
        sscanf(&line[FIELD1], "%d" , &stock[n].day);
        sscanf(&line[FIELD2], "%lf", &stock[n].price);
        sscanf(&line[FIELD3], "%4s", stock[n].ticker);  // max len: 4+1=5 chars
        stock[n].key = (int)round(stock[n].price * 100);  // max len: 3+2=5 digits
        sprintf(stock[n].digits, "%d", stock[n].key);  // max len: 5+1=6 chars
    }
    fclose(f);
    return n;
}

static int64_t secret(const int days)
{
    if (days <= 0)
        return 0;

    // Find first manipulated stock
    int i = 0;
    for (; i < days && !strstr(pi32, stock[i].digits); ++i)
        clean[cleanlen++] = &stock[i];  // mark as clean
    if (i == days)
        return 0;  // all records are clean
    double fval = stock[i].price;  // initial secret value
    manip[maniplen++] = &stock[i++];  // mark as manipulated, go to next index

    // Find other manipulated stocks
    for (; i < days; ++i)
        if (strstr(pi32, stock[i].digits)) {
            switch (stock[i].day & 1) {
                case 0: fval *= stock[i].price; break;  // even day: multiply
                case 1: fval /= stock[i].price; break;  // odd day: divide
            }
            manip[maniplen++] = &stock[i];  // mark as manipulated
        } else
            clean[cleanlen++] = &stock[i];  // mark as clean

    // First 10 digits of floating point value
    int64_t ival = 0;
    char buf[32];
    sprintf(buf, "%.10e", fval);  // 1 unit + 10 decimals = 11 digits total, so no rounding of 10th digit
    for (int i = 0, count = 0; buf[i] && count < 10; ++i)
        if ((buf[i] >= '1' && buf[i] <= '9') || (buf[i] == '0' && count)) {  // only select digits, skip leading zeros
            ival = ival * 10 + (buf[i] & 15);
            ++count;
        }
    return ival;
}

static inline void shift(char *str, const int key)
{
    for (; *str; str++)
        *str = 'A' + (*str - 'A' + key) % ALPHLEN;
}

static int cmp_day(const void *p, const void *q)
{
    const int a = ((const Stock *const)p)->day;
    const int b = ((const Stock *const)q)->day;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

int main(void)
{
    int stocklen = parse(FNAME);
    printf("Part 1: %lld\n", secret(stocklen));  // 6361428769

    FILE *f = fopen("piday2025-2.txt", "r");
    if (!f) return 2;
    char ciphermap[256];
    for (int i = 0, c = fgetc(f); i < 256 && c >= 0; c = fgetc(f))
        if (c >= 'A' && c <= 'Z')
            ciphermap[i++] = c;
    fclose(f);

    for (int i = 0; i < maniplen; ++i) {
        shift(manip[i]->ticker, manip[i]->key);  // shift ticker names of manipulated stock
        for (int j = 0; j < cleanlen; ++j)
        if (!strcmp(manip[i]->ticker, clean[j]->ticker)) {  // match shifted names to clean stock names
            phrase[phraselen++] = clean[j];
            break;
        }
    }
    qsort(phrase, phraselen, sizeof *phrase, cmp_day);

    printf("Part 2: ");
    for (int i = 0; i < phraselen; ++i)
        fputc(ciphermap[phrase[i]->key % 256], stdout);
    printf("\n");  // GOODPIDAY

    return 0;
}

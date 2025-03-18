/**
 * https://ivanr3d.com/projects/pi/2025.html
 * https://www.reddit.com/r/adventofcode/comments/1jb2eis/pi_coding_quest_2025/
 */

#include <stdio.h>     // FILE, fopen, fclose, fgets, printf, sprintf, sscanf, fputc
#include <stdlib.h>    // qsort
#include <string.h>    // strstr
#include <math.h>      // round
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>

// Number of letters in the ASCII alphabet
#define ALPHLEN ('Z' - 'A' + 1)

// Data file specification part 1
#define FNAME1  "piday2025.txt"
#define MAXDAYS  30  // number of records (days)
#define LINELEN  64  // size of buffer guaranteed to fit any line (actual: 39 or 40)
#define FIELD1    0  // column index 1 of fixed width data: day
#define FIELD2   13  // column index 2 of fixed width data: price
#define FIELD3   32  // column index 3 of fixed width data: ticker
#define PRECIS  100  // price precision (= factor to get integer)

// Cipher map specification part 2
#define FNAME2 "piday2025-2.txt"
#define MAPDIM 16  // square grid of 16x16 letters
#define MAPLEN (MAPDIM * MAPDIM)  // 256 letters

// First 32 digits (= 31 decimals) of pi
static const char *pi32 = "31415926535897932384626433832795";

typedef struct stock {
    int day;         // unique day number
    int key;         // price as integer for shift cipher
    double price;    // price as float for calculation
    char digits[8];  // price as digit string for searching
    char ticker[8];  // stock name
} Stock;

// Part 1
static Stock stock[MAXDAYS];   // records from data file +extra info
static Stock *manip[MAXDAYS];  // pointers to manipulated stock
static Stock *clean[MAXDAYS];  // pointers to clean stock
static int maniplen, cleanlen;

// Part 2
static char ciphermap[MAPLEN];

// Read stock records from fixed width data file (part 1)
// Return number of lines read
static int parse_data(const char *fname)
{
    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;
    char line[LINELEN];
    fgets(line, sizeof line, f);  // discard first line (= table header)
    int n = 0;  // number of lines parsed (= days/stocks)
    for (; n < MAXDAYS && fgets(line, sizeof line, f); ++n) {
        sscanf(&line[FIELD1], "%2d" , &stock[n].day);        // range: 0-99
        sscanf(&line[FIELD2], "%6lf", &stock[n].price);      // range: 0.00-999.99
        sscanf(&line[FIELD3], "%4s", stock[n].ticker);       // max len: 4+1=5 chars
        stock[n].key = (int)round(stock[n].price * PRECIS);  // max len: 3+2=5 digits
        sprintf(stock[n].digits, "%d", stock[n].key);        // max len: 5+1=6 chars
    }
    fclose(f);
    return n;
}

static int64_t secret(const int days)
{
    if (days <= 0)
        return 0;

    // Find first manipulated stock
    int n = cleanlen = maniplen = 0;  // setting lengths to zero enables multiple calls
    for (; n < days && !strstr(pi32, stock[n].digits); ++n)
        clean[cleanlen++] = &stock[n];  // mark as clean
    if (n == days)
        return 0;  // all records are clean
    double fval = stock[n].price;  // initial secret value
    manip[maniplen++] = &stock[n];  // mark as manipulated

    // Find other manipulated stocks
    while (++n < days)
        if (strstr(pi32, stock[n].digits)) {
            switch (stock[n].day & 1) {
                case 0: fval *= stock[n].price; break;  // even day: multiply
                case 1: fval /= stock[n].price; break;  // odd day: divide
            }
            manip[maniplen++] = &stock[n];  // mark as manipulated
        } else
            clean[cleanlen++] = &stock[n];  // mark as clean

    // First 10 digits of floating point value
    int64_t intval = 0;
    char buf[32];
    sprintf(buf, "%.10e", fval);  // 1 unit + 10 decimals = 11 significant digits total, so no rounding of 10th digit
    for (int i = 0, count = 0; buf[i] && count < 10; ++i)
        if ((buf[i] >= '1' && buf[i] <= '9') || (buf[i] == '0' && count)) {  // only select digits, skip leading zeros
            intval = intval * 10 + (buf[i] & 15);
            ++count;
        }
    return intval;
}

// Read cipher map from file (part 2)
// Return number of letters read
static int parse_map(const char *fname)
{
    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;
    char line[LINELEN];
    int k = 0;
    for (int i = 0; i < MAPDIM; ++i)
        if (fgets(line, sizeof line, f))
            for (int j = 0; j < MAPDIM; ++j)
                ciphermap[k++] = line[j * 2];  // letters separated by 1 space
    fclose(f);
    return k;
}

// Apply shift cipher (additive/right) to whole string
static inline void shiftright(char *str, const int key)
{
    for (; *str; str++)
        *str = 'A' + (*str - 'A' + key) % ALPHLEN;
}

// Qsort helper function.
// p and q are pointers to stock pointers
static int cmp_day(const void *p, const void *q)
{
    const int a = (*(const Stock **)p)->day;
    const int b = (*(const Stock **)q)->day;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static char *decipher(const int maplen)
{
    static char result[MAXDAYS + 1] = "";

    if (maplen <= 0)
        return NULL;

    Stock *phrase[MAXDAYS];
    int phraselen = 0;
    for (int i = 0; i < maniplen; ++i) {
        shiftright(manip[i]->ticker, manip[i]->key);  // shift ticker names of manipulated stock
        for (int j = 0; j < cleanlen; ++j)
            if (!strcmp(manip[i]->ticker, clean[j]->ticker)) {  // match shifted names to clean stock names
                phrase[phraselen++] = clean[j];  // save pointer to new array
                break;
            }
    }
    qsort(phrase, phraselen, sizeof *phrase, cmp_day);  // sort matched clean stock by day(!!!)
    for (int i = 0; i < phraselen; ++i)
        result[i] = ciphermap[phrase[i]->key % maplen];
    result[phraselen] = '\0';
    return result;
}

int main(void)
{
    printf("Part 1: %"PRId64"\n", secret(parse_data(FNAME1)));  // 6361428769
    printf("Part 2: %s\n", decipher(parse_map(FNAME2)));        // GOODPIDAY
    return 0;
}

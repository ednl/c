#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FNAME "piday2025.txt"
#define DAYS     30
#define COLPRICE 13
#define COLNAME  32

static const char *pi32 = "31415926535897932384626433832795";

typedef struct stock {
    int day;
    double price;
    char digits[8];
    char ticker[8];
} Stock;

static Stock stock[DAYS];

static double hiddenval(void)
{
    int i = 0;
    while (i < DAYS && !strstr(pi32, stock[i].digits))
        ++i;
    if (i == DAYS)
        return 0;
    // printf("%d: %.2f\n", stock[i].day, stock[i].price);
    double secret = stock[i++].price;
    for (; i < DAYS; ++i)
        if (strstr(pi32, stock[i].digits)) {
            if (stock[i].day & 1) {
                secret /= stock[i].price;
                // printf("%d: / %.2f = %.14f\n", stock[i].day, stock[i].price, secret);
            } else {
                secret *= stock[i].price;
                // printf("%d: * %.2f = %.14f\n", stock[i].day, stock[i].price, secret);
            }
        }
    return secret;
}

int main(void)
{
    // Open file
    FILE *f = fopen(FNAME, "r");
    if (!f) return 1;

    // Parse file
    char line[64];
    fgets(line, sizeof line, f);  // discard first line
    for (int i = 0; i < DAYS && fgets(line, sizeof line, f); ++i) {
        stock[i].day = i + 1;
        sscanf(&line[COLPRICE], "%lf", &stock[i].price);
        for (int j = COLPRICE, k = 0; line[j] != ' '; ++j)
            if (line[j] != '.')
                stock[i].digits[k++] = line[j];  // already NUL terminated from static init
        sscanf(&line[COLNAME], "%4s", stock[i].ticker);
    }
    fclose(f);

    // Show data
    // for (int i = 0; i < DAYS; ++i)
    //     printf("%-2d %-5s %-4s %.2f\n", stock[i].day, stock[i].digits, stock[i].ticker, stock[i].price);

    char buf[32];
    sprintf(buf, "%.10e", hiddenval());  // 11 digits total, so no rounding of 10th digit
    int64_t secret = 0;
    for (int i = 0, count = 0; buf[i] && count < 10; ++i)
        if (buf[i] >= '0' && buf[i] <= '9') {
            secret = secret * 10 + (buf[i] & 15);
            ++count;
        }
    printf("%s = %lld\n", buf, secret);

    return 0;
}

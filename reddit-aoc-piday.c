#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define L ('z' - 'a' + 1)

static const char msg[] = "Wii kxtszof ova fsegyrpm d lnsrjkujvq roj! Kdaxii svw vnwhj pvugho buynkx tn vwh-gsvw ruzqia. Mrq'x kxtmjw bx fhlhlujw cjoq! Hmg tyhfa gx dwd fdqu bsm osynbn oulfrex, kahs con vjpmd qtjv bx whwxssp cti hmulkudui f Jgusd Yp Gdz!";
static const char pi[] = "314159265358979323846264338327950288";
static const char* num[] = {"two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};

int main(void)
{
    const char* s = msg;
    char letters[256] = {0};
    int count = 0;
    for (int i = 0; *s; ++i, ++s) {
        char c = *s;
        if ((c >= 'A' && c <= 'Z')) {
            c = 'A' + (L + c - 'A' - pi[i & 15] + '0') % L;
            letters[count++] = c - 'A' + 'a';
        } else if (c >= 'a' && c <= 'z') {
            c = 'a' + (L + c - 'a' - pi[i & 15] + '0') % L;
            letters[count++] = c;
        }
        putc(c, stdout);
    }
    printf("\n%s\n", letters);
    int m = 1;
    for (int i = 0; i < (int)(sizeof num / sizeof *num); ++i) {
        const char* t = letters;
        while (t && *t) {
            if (t = strstr(t, num[i])) {
                m *= i + 2;
                t += strlen(num[i]);
            }
        }
    }
    printf("%d", m);
    return 0;
}

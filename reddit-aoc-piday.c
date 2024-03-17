#include <stdio.h>
#include <string.h>  // strstr

#define ALPH ('z' - 'a' + 1)  // 26 letters
#define CASE 32  // lowercase = uppercase | 32

static const char pi[] = "314159265358979323846264338327950288";
static const char* num[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
static char msg[] = "Wii kxtszof ova fsegyrpm d lnsrjkujvq roj! Kdaxii svw vnwhj pvugho buynkx tn vwh-gsvw ruzqia. Mrq'x kxtmjw bx fhlhlujw cjoq! Hmg tyhfa gx dwd fdqu bsm osynbn oulfrex, kahs con vjpmd qtjv bx whwxssp cti hmulkudui f Jgusd Yp Gdz!";

static inline char shift(const char letter, const char base, const int index)
{
    return base + (ALPH + letter - base - (pi[index & 15] & 15)) % ALPH;
}

int main(void)
{
    char letters[256] = {0};
    int i = 0, count = 0;
    for (char* s = msg; *s; ++s, ++i)
        if (*s >= 'a' && *s <= 'z')
            letters[count++] = *s = shift(*s, 'a', i);
        else if ((*s >= 'A' && *s <= 'Z'))
            letters[count++] = (*s = shift(*s, 'A', i)) | CASE;
    printf("Part 1: %s\n", msg);

    int prod = 1;
    for (int n = 2; n <= 10; ++n) {
        const size_t len = strlen(num[n]);
        for (const char* s = letters; (s = strstr(s, num[n])); s += len)
            prod *= n;
    }
    printf("Part 2: %d\n", prod);
    return 0;
}

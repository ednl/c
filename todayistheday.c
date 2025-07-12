#include <stdio.h>
#include <time.h>
#include <string.h>
int main(void) {
    const char *ref = "07/12/2025";
    const time_t now = time(NULL);
    const struct tm *t = localtime(&now);
    char today[16];
    sprintf(today, "%02d/%02d/%d",
        1 + t->tm_mon, t->tm_mday, 1900 + t->tm_year);
    if (!strcmp(ref, today))
        printf("Today is the day.\n");
}

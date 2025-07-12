#include <stdio.h>
#include <time.h>
#include <string.h>
int main(void) {
    char today[9], *theday = "07/12/25";
    const time_t t = time(NULL);
    strftime(today, sizeof today, "%D", localtime(&t));
    if (!strcmp(today, theday))
        printf("Today is the day.\n");
}

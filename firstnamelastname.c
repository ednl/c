#include <stdio.h>
#include <string.h>

int main(void)
{
    char buf[256], *first, *last;

    fputs("Name? ", stdout);
    if (fgets(buf, sizeof buf, stdin)
        && (first = strtok(buf, " "))
        && (last = strtok(NULL, " \n")))
        printf("%s, %c.\n", last, *first);

    return 0;
}

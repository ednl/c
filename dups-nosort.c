#include <stdio.h>
static char a[BUFSIZ];
int main(void)
{
    fgets(a, sizeof a, stdin);
    int dup = 0;
    for (int i = 0; a[i]; ++i)
        for (int j = 0, seen = 0; a[j]; ++j) {
            seen += i > j && a[i] == a[j];
            dup += !seen && i < j && a[i] == a[j];
        }
    printf("%d\n", dup);
}

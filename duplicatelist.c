#include <stdio.h>
#include <stdlib.h>  // malloc, free, srandom, random, qsort, bsearch
#include <time.h>

typedef struct node {
    struct node *next;
    struct node *ref;
} Node;

static int randomint(const int min, const int max)
{
    const int len = max - min + 1;
    int x = len;
    while (x == len)
        x = random() / ((RAND_MAX + 1u) / len);
    return x + min;
}

static Node *duplicate(Node *list)
{
    return NULL;
}

int main(void)
{
    srandom(time(NULL));
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 50; ++j)
            printf(" %d", randomint(1, 6));
        printf("\n");
    }
    return 0;
}

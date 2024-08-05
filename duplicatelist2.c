#include <stdio.h>
#include <stdlib.h>  // malloc, free
#include <stdint.h>  // uintptr_t

typedef struct node {
    struct node *next;
    struct node *ref;
} Node;

static int randint(const int range)
{
    int x = range;
    while (x == range)
        x = random() / ((RAND_MAX + 1U) / range);
    return x;
}

static Node *genlist(const int n)
{
    int *a = malloc(n * sizeof *a);
    for (int i = 0; i < n; ++i)
        a[i] = i;
    for (int i = 0; i < n; ++i) {
        const int j = randint(n);
        const int k = a[i];
        a[i] = a[j];
        a[j] = k;
    }
    free(a);
    return NULL;
}

int main(void)
{
    int x;
    while ((x = randint(6)) < 6);
    printf("%d\n", x);
    return 0;
}

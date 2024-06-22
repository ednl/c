#include <stdio.h>
#include <stdlib.h>  // malloc, free, srandom, random, qsort, bsearch
#include <time.h>

typedef struct node {
    struct node *next;
    struct node *ref;
} Node;

typedef struct ref {
    const Node *node;
    int index, iref;
} Ref;

static int randomint(const int min, const int max)
{
    const int len = max - min + 1;
    int x = len;
    while (x == len)
        x = random() / ((RAND_MAX + 1u) / len);
    return x + min;
}

static Node *list_from_refs(const int count, const int *const refs)
{
    // Array of Node pointers
    Node **pnode = malloc(count * sizeof(Node*));
    if (!pnode)
        return NULL;
    // Allocate nodes
    for (int i = 0; i < count; ++i)
        if (!(pnode[i] = malloc(sizeof(Node))))
            return NULL;
    // Set nodes except last
    for (int i = 0; i < count - 1; ++i)
        *pnode[i] = (Node){.next = pnode[i + 1], .ref = pnode[refs[i]]};
    // Set last node
    *pnode[count - 1] = (Node){.next = NULL, .ref = pnode[refs[count - 1]]};
    Node *head = pnode[0];
    free(pnode);
    return head;
}

static Node *generate_list(const int count)
{
    // Array of indices in range [0..count-1]
    int *iref = malloc(count * sizeof(int));
    if (!iref)
        return NULL;
    for (int i = 0; i < count; ++i)
        iref[i] = i;
    // Shuffle
    for (int i = 0; i < count; ++i) {
        int j = randomint(0, count - 2);
        if (j >= i)
            ++j;
        const int k = iref[i];
        iref[i] = iref[j];
        iref[j] = k;
    }
    Node *head = list_from_refs(count, iref);
    free(iref);
    return head;
}

static void print_list(const Node *head)
{
    for (int i = 0; head; ++i, head = head->next)
        printf("%d: node=%p next=%p ref=%p\n", i, head, head->next, head->ref);
}

static void delete_list(Node *head)
{
    while (head) {
        Node *next = head->next;
        free(head);
        head = next;
    }
}

static int cmp_refnode(const void *p, const void *q)
{
    const Ref *const a = p;
    const Ref *const b = q;
    if (a->node < b->node) return -1;
    if (a->node > b->node) return 1;
    return 0;
}

static int cmp_refindex(const void *p, const void *q)
{
    const Ref *const a = p;
    const Ref *const b = q;
    if (a->index < b->index) return -1;
    if (a->index > b->index) return 1;
    return 0;
}

static Node *duplicate_list(const Node *const oldlist)
{
    // Count nodes in list
    int count = 0;
    for (const Node *n = oldlist; n; ++count, n = n->next);
    if (!count)
        return NULL;
    // Inventory of internal references
    Ref *ref = malloc(count * sizeof(Ref));
    if (!ref)
        return NULL;
    const Node *n = oldlist;
    for (int i = 0; i < count; ++i, n = n->next)
        ref[i] = (Ref){.node = n, .index = i};  // .iref TBD, see below
    // Sort by node pointer so we can use bsearch on them
    qsort(ref, count, sizeof *ref, cmp_refnode);
    // Look up each reference pointer in the array of node pointers
    for (int i = 0; i < count; ++i) {
        const Ref key = (Ref){.node = ref[i].node->ref};
        const Ref *const r = bsearch(&key, ref, count, sizeof *ref, cmp_refnode);
        if (r)
            ref[i].iref = r->index;  // the ref pointer points to which index
    }
    // Sort by original index
    qsort(ref, count, sizeof *ref, cmp_refindex);
    // Ref indices to separate array
    int *iref = malloc(count * sizeof(int));
    if (!iref)
        return NULL;
    for (int i = 0; i < count; ++i)
        iref[i] = ref[i].iref;
    Node *newlist = list_from_refs(count, iref);
    free(iref);
    free(ref);
    return newlist;
}

int main(void)
{
    srandom(time(NULL));

    Node *list1 = generate_list(5);
    if (!list1)
        return 1;
    print_list(list1);

    Node *list2 = duplicate_list(list1);
    if (!list2)
        return 2;
    print_list(list2);

    delete_list(list1);
    delete_list(list2);
    return 0;
}

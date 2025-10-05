/**
 * Compile with clang 21.1.0 on 64-bit ARM (armv8-a):
 *     clang -std=c90 -Wall -Wextra -pedantic -O2
 *
 * Warnings:
 *     <source>:6:34: warning: format specifies type 'void *' but the argument has type 'char (*)[10]' [-Wformat-pedantic]
 *         6 |     printf("address  a : %p\n",  a);
 *           |                          ~~      ^
 *     <source>:15:43: warning: sizeof on array function parameter will return size of 'char *' instead of 'char[10]' [-Wsizeof-array-argument]
 *        15 |     printf("sizeof   b : %lu\n", sizeof   b);
 *           |                                           ^
 *     <source>:4:34: note: declared here
 *         4 | static void f(char (*a)[N], char b[N], char *c, char d)
 *           |                                  ^
 *     2 warnings generated.
 *     Compiler returned: 0
 *
 * Output:
 *     s = "sssss"
 *     address s : 0x1008e8778
 *     strlen(s) : 5
 *     sizeof s  : 8
 *
 *     t = "ttttt"
 *     address t : 0x16f516ee0
 *     strlen(t) : 5
 *     sizeof t  : 6
 *
 *     u = "uuuuu"
 *     address u : 0x16f516ee8
 *     strlen(u) : 5
 *     sizeof u  : 10
 *
 *     address  a : 0x16f516ee8
 *     address *a : 0x16f516ee8
 *     address  b : 0x16f516ee8
 *     address  c : 0x16f516ee8
 *     address &d : 0x16f516ee7
 *
 *     sizeof   a : 8
 *     sizeof  *a : 10
 *     sizeof **a : 1
 *     sizeof   b : 8
 *     sizeof  *b : 1
 *     sizeof   c : 8
 *     sizeof  *c : 1
 *     sizeof   d : 1
 *
 *     u = "abcuu"
 */
#include <stdio.h>
#include <string.h>
#define N 10
static void f(char (*a)[N], char b[N], char *c, char d)
{
    printf("address  a : %p\n",  a);
    printf("address *a : %p\n", *a);
    printf("address  b : %p\n",  b);
    printf("address  c : %p\n",  c);
    printf("address &d : %p\n", &d);
    printf("\n");
    printf("sizeof   a : %lu\n", sizeof   a);
    printf("sizeof  *a : %lu\n", sizeof  *a);
    printf("sizeof **a : %lu\n", sizeof **a);
    printf("sizeof   b : %lu\n", sizeof   b);
    printf("sizeof  *b : %lu\n", sizeof  *b);
    printf("sizeof   c : %lu\n", sizeof   c);
    printf("sizeof  *c : %lu\n", sizeof  *c);
    printf("sizeof   d : %lu\n", sizeof   d);
    (*a)[0] = 'a';
    b[1] = 'b';
    c[2] = 'c';
    d = 'd';
}
int main(void)
{
    char *s   = "sssss";
    char t[]  = "ttttt";
    char u[N] = "uuuuu";
    printf("s = \"%s\"\n", s);
    printf("address s : %p\n",  s);
    printf("strlen(s) : %lu\n", strlen(s));
    printf("sizeof s  : %lu\n", sizeof s);
    printf("\n");
    printf("t = \"%s\"\n", t);
    printf("address t : %p\n",  t);
    printf("strlen(t) : %lu\n", strlen(t));
    printf("sizeof t  : %lu\n", sizeof t);
    printf("\n");
    printf("u = \"%s\"\n", u);
    printf("address u : %p\n",  u);
    printf("strlen(u) : %lu\n", strlen(u));
    printf("sizeof u  : %lu\n", sizeof u);
    printf("\n");
    f(&u, u, &u[0], u[3]);
    printf("\n");
    printf("u = \"%s\"\n", u);
    return 0;
}

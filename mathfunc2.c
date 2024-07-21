#include <stdio.h>   // printf, fgets, scanf
#include <ctype.h>   // isspace
#include <string.h>  // strspn

// Only returns after valid input of a single operator
// (optionally preceded by whitespace)
static char getop(void)
{
    static char *valid = "+-*/";
    char buf[BUFSIZ];
    const char *s;
    while (1) {
        printf("Function? (+,-,*,/) ");
        if ((s = fgets(buf, sizeof buf, stdin))) {
            while (isspace(*s))
                ++s;
            if (strspn(s, valid) == 1 && (*(s + 1) == '\r' || *(s + 1) == '\n' || *(s + 1) == '\0'))
                return *s;
        }
    }
}

// Calculate "a op b" where op in "+-*/", otherwise returns zero
static double calc(const char op, const double a, const double b)
{
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
    return 0;
}

int main(void)
{
    const char op = getop();

    double a;
    printf("Number 1? ");
    if (scanf("%lf", &a) != 1)
        return 1;

    double b;
    printf("Number 2? ");
    if (scanf("%lf", &b) != 1)
        return 2;

    printf("%f %c %f = %f\n", a, op, b, calc(op, a, b));
    return 0;
}

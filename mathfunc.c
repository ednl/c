#include <stdio.h>    // scanf, fprintf
#include <stdlib.h>   // EXIT_FAILURE
#include <stdbool.h>  // bool, true, false

// First enum value must be zero
typedef enum fun {
    ADD, SUB, MUL, DIV
} Fun;

// Declare for use in function pointer array
static double add(const double, const double);
static double sub(const double, const double);
static double mul(const double, const double);
static double dvd(const double, const double);  // 'div' already defined

// Must be same order as 'enum fun'
static double (*funptr[])(const double, const double) = {add, sub, mul, dvd};

// Functions definitions
static double add(const double a, const double b)
{
    return a + b;
}

static double sub(const double a, const double b)
{
    return a - b;
}

static double mul(const double a, const double b)
{
    return a * b;
}

static double dvd(const double a, const double b)
{
    return a / b;
}

// Read function symbol, return true if success
static bool getfun(Fun *fun)
{
    char c;
    printf("Function? ");
    if (scanf("%c", &c) != 1)  // warning: leaves newline in read buffer
        return false;
    switch (c) {
        case '+': *fun = ADD; break;
        case '-': *fun = SUB; break;
        case '*': *fun = MUL; break;
        case '/': *fun = DIV; break;
        default: return false;
    }
    return true;
}

// Read number, return true if success
static bool getnum(double *num, const int index)
{
    printf("Number %d? ", index);
    return scanf("%lf", num) == 1;  // leading whitespace automatically absorbed
}

int main(void)
{
    Fun fun;
    double num[2] = {0};

    if (!getfun(&fun)) {
        fprintf(stderr, "No valid function symbol.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < 2; ++i)
        if (!getnum(&num[i], i + 1)) {
            fprintf(stderr, "Number %d not valid.\n", i + 1);
            return EXIT_FAILURE;
        }
    printf("Result  : %f\n", funptr[fun](num[0], num[1]));
    return 0;
}

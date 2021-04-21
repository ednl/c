#include <stdio.h>

// Enum type starting from zero => unsigned int
enum EnumPos
{
    EP_ZERO = 0,
    EP_POS1,
    EP_POS2
};

// Enum type starting from -1 => signed int
enum EnumNeg
{
    EN_NEG = -1,
    EN_ZERO = 0,
    EN_POS1,
    EN_POS2
};

// Return an enum type (which is unsigned int)
static enum EnumPos getpos(void)
{
    return EP_POS1;  // value should be +1 (unsigned int)
}

// Return an enum type (which is signed int)
static enum EnumNeg getneg(void)
{
    return EN_POS1;  // value should be +1 (signed int)
}

// Receive and return the same enum type (which is signed int)
static enum EnumNeg retneg(enum EnumNeg e)
{
    return e;
}

int main(void)
{
    // Declare variables explicitly as signed or unsigned
    signed   int i;
    unsigned int u;

    printf("EnumPos as unsigned: +%u\n", (u = getpos()));
    printf("EnumPos as   signed: %+i\n", (i = getpos()));  // COMPILER WARNING: changes signedness
    printf("EnumNeg as unsigned: +%u\n", (u = getneg()));  // COMPILER WARNING: changes signedness
    printf("EnumNeg as   signed: %+i\n", (i = getneg()));
    // 2 compiler warnings, output:
    // EnumPos as unsigned: +1
    // EnumPos as   signed: +1
    // EnumNeg as unsigned: +1
    // EnumNeg as   signed: +1

    enum EnumNeg e = EN_POS2;
    printf("Define as EnumNeg     Pass as EnumNeg     Return as EnumNeg  Print as signed int: %+i\n", retneg(e));
    signed int i2 = 2;
    printf("Define as signed int  Pass as signed int  Return as EnumNeg  Print as signed int: %+i\n", retneg(i2));
    signed int i3 = EN_POS2;
    printf("Define as EnumNeg     Pass as signed int  Return as EnumNeg  Print as signed int: %+i\n", retneg(i3));
    // 0 compiler warnings, output:
    // Define as EnumNeg     Pass as EnumNeg     Return as EnumNeg  Print as signed int: +2
    // Define as signed int  Pass as signed int  Return as EnumNeg  Print as signed int: +2
    // Define as EnumNeg     Pass as signed int  Return as EnumNeg  Print as signed int: +2

    enum EnumNeg e2 = 3;                                   // COMPILER WARNING: integer constant not in range
    printf("Invalid enum value: %i\n", retneg(e2));
    // 1 compiler warning, output:
    // Invalid enum value: 3

    return 0;
}

#include <stdio.h>

enum EnumPos
{
    EP_ZERO = 0,
    EP_POS1,
    EP_POS2
};

enum EnumNeg
{
    EN_NEG = -1,
    EN_ZERO = 0,
    EN_POS1,
    EN_POS2
};

static enum EnumPos getpos(void)
{
    return EP_POS1;  // value should be +1 (unsigned int)
}

static enum EnumNeg getneg(void)
{
    return EN_POS1;  // value should be +1 (signed int)
}

static enum EnumNeg retneg(enum EnumNeg e)
{
    return e;
}

int main(void)
{
    int i;
    unsigned int u;

    printf("EnumPos as unsigned: +%u\n", (u = getpos()));
    printf("EnumPos as   signed: %+i\n", (i = getpos()));  // COMPILER WARNING: changes signedness

    printf("EnumNeg as unsigned: +%u\n", (u = getneg()));  // COMPILER WARNING: changes signedness
    printf("EnumNeg as   signed: %+i\n", (i = getneg()));

    /* Output:
    EnumPos as unsigned: +1
    EnumPos as   signed: +1
    EnumNeg as unsigned: +1
    EnumNeg as   signed: +1
    */

    enum EnumNeg e = EN_POS2;
    printf("Define as EnumNeg     Pass as EnumNeg     Return as EnumNeg  Print as signed int: %+i\n", retneg(e));

    int i2 = 2;
    printf("Define as signed int  Pass as signed int  Return as EnumNeg  Print as signed int: %+i\n", retneg(i2));

    int i3 = EN_POS2;
    printf("Define as EnumNeg     Pass as signed int  Return as EnumNeg  Print as signed int: %+i\n", retneg(i3));

    /* Output:
    Define as EnumNeg     Pass as EnumNeg     Return as EnumNeg  Print as signed int: +2
    Define as signed int  Pass as signed int  Return as EnumNeg  Print as signed int: +2
    Define as EnumNeg     Pass as signed int  Return as EnumNeg  Print as signed int: +2
    */

    return 0;
}

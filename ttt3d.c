#include <stdio.h>
#include <stdlib.h>

#define BOARDSZ  (3)
#define SQ_MASK  (0x0f)
#define SZ_MASK  (0x03)
#define PL_MASK  (0x01)
#define SQ_SHIFT (0)
#define SZ_SHIFT (4)
#define PL_SHIFT (6)

typedef struct move {
    char player, square, size;
} Move, *pMove;

typedef enum piece {
    SMALL,
    MEDIUM,
    LARGE,
} piece_t;

// typedef struct piece {
// }

static char move2char(Move m)
{
    return (char)(
        ((m.player & PL_MASK) << PL_SHIFT) |
        ((m.square & SQ_MASK) << SQ_SHIFT) |
        ((m.size   & SZ_MASK) << SZ_SHIFT)
    );
}

static Move char2move(char c)
{
    return (Move){
        .player = (c >> PL_SHIFT) & PL_MASK,
        .square = (c >> SQ_SHIFT) & SQ_MASK,
        .size   = (c >> SZ_SHIFT) & SZ_MASK,
    };
}

static char stack[1024] = {0};

int main(void)
{
    char player = 0;
    char sqfree = 3 * 3;
    while(sqfree && ) {
        player ^= 1;
    }

    return 0;
}

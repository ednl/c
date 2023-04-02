// Puzzle: Implement a fantasy computer to find out the answer to this program
// https://www.reddit.com/r/adventofcode/comments/128t3c6/puzzle_implement_a_fantasy_computer_to_find_out/
// Puzzle designed by https://www.reddit.com/user/codeobserver for https://codeguppy.com
// Solution by E. Dronkert https://github.com/ednl

#include <stdio.h>   // printf
#include <stdlib.h>  // malloc, free

// VM stack size (max size used for this puzzle: 2)
#define STACKSZ 16

// Program code
static const char *input = "11,0,10,42,6,255,30,0,11,0,0,11,1,1,11,3,1,60,1,10,2,0,20, 2,1,60,2,10,0,1,10,1,2,11,2,1,20,3,2,31,2,30,2,41,3,2,19,31,0,50";

// Global vars (auto init to 0)
static int *mem;
static int stack[STACKSZ], sp;
static int R[4];

static int run(void)
{
    int ip = 0, clock = 0;
    while (1) {
        ++clock;
        switch (mem[ip++]) {
            case  10: R[mem[ip]]  = R[mem[ip + 1]]; ip += 2; break;  // MOV   rdst rsrc
            case  11: R[mem[ip]]  =   mem[ip + 1];  ip += 2; break;  // MOV   rdst val
            case  20: R[mem[ip]] += R[mem[ip + 1]]; ip += 2; break;  // ADD   rdst rsrc
            case  21: R[mem[ip]] -= R[mem[ip + 1]]; ip += 2; break;  // SUB   rdst rsrc
            case  30: stack[sp++]  = R[mem[ip++]];           break;  // PUSH  rsrc
            case  31: R[mem[ip++]] = stack[--sp];            break;  // POP   rdst
            case  40: ip = mem[ip];                          break;  // JP    addr
            case  41: ip = R[mem[ip]] < R[mem[ip + 1]] ? mem[ip + 2] : ip + 3; break;  // JL reg1 reg2 addr
            case  42: stack[sp++] = ip + 1; ip = mem[ip];    break;  // CALL  addr (push ret-addr)
            case  50: ip = stack[--sp];                      break;  // RET   (pop ret-addr)
            case  60: printf("%d\n", R[mem[ip++]]);          break;  // PRINT reg
            case 255: return clock;                                  // HALT
        }
    }
}

int main(void)
{
    // Count values in CSV data
    size_t codesize = 1;
    const char *c = input;
    while (*c)
        if (*c++ == ',')
            ++codesize;

    // Allocate memory
    mem = malloc(codesize * sizeof *mem);
    if (!mem)
        exit(1);

    // Parse CSV data to memory
    c = input;
    for (int i = 0; i < codesize; ++i) {
        sscanf(c, "%d", &mem[i]);
        while (*c && *c != ',')
            ++c;
        if (*c == ',')
            ++c;
    }

    // Run & clean up
    // Executes 100 instructions, prints first 10 numbers of Fibonacci sequence
    printf("clock=%d\n", run());
    free(mem);
    return 0;
}

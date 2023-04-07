// Puzzle: Implement a fantasy computer to find out the answer to this program
// https://www.reddit.com/r/adventofcode/comments/128t3c6/puzzle_implement_a_fantasy_computer_to_find_out/
// Puzzle designed by https://www.reddit.com/user/codeobserver for https://codeguppy.com
// Solution by E. Dronkert https://github.com/ednl

#include <stdio.h>    // printf, sscanf
#include <stdlib.h>   // malloc, free
#include <stdbool.h>  // bool
#include <stdint.h>   // int64_t
#include <limits.h>   // LONG_WIDTH

#define MEMORYSIZE  64  // actually used by puzzle: 50
#define STACKSIZE   16  // actually used by puzzle: 2
#define REGISTERS    4  // given by puzzle
#define MAXPARCOUNT  3  // instructions have 0 to 3 parameters

// Valid opcode range
#define OPCODE_MIN  10
#define OPCODE_MAX 255

// Opcode parameter addressing modes
#define PAR_IMMEDIATE 1  // parameter is a direct value
#define PAR_REGISTER  2  // parameter is a register index
#define PAR_ABSOLUTE  3  // parameter is an absolute memory address
// #define PAR_RELATIVE  4  // unused

#define ERR_OK                0
#define ERR_NULLPOINTER       1
#define ERR_INVALID_OPCODE    2
#define ERR_PC_UNDERFLOW      3
#define ERR_PC_OVERFLOW       4
#define ERR_STACK_FULL        5
#define ERR_STACK_EMPTY       6
#define ERR_REGISTER_INDEX    7
#define ERR_INTERNAL_PARCOUNT 8
#define ERR_INTERNAL_PARMODE  9

// Program code
static const char *input = "11,0,10,42,6,255,30,0,11,0,0,11,1,1,11,3,1,60,1,10,2,0,20,2,1,60,2,10,0,1,10,1,2,11,2,1,20,3,2,31,2,30,2,41,3,2,19,31,0,50";

// Instruction
// parmode: units = addr mode of 1st par, tens = addr mode of 2nd par, etc.
// stackdir: +1 = wants to push onto the stack, -1 = wants to pop off the stack
typedef struct _Instr {
    int opcode, parmode, stackdir;
    char name[8];
} Instr;

static Instr lang[26][6] = {
    {0},
    {{10, 22,  0, "MOVR" }, {11,  12,  0, "MOVV"}},
    {{20, 22,  0, "ADD"  }, {21,  22,  0, "SUB" }},
    {{30,  2,  1, "PUSH" }, {31,   2, -1, "POP" }},
    {{40,  3,  0, "JP"   }, {41, 322,  0, "JL"  }, {42, 3, 1, "CALL"}},
    {{50,  0, -1, "RET"  }},
    {{60,  2,  0, "PRINT"}}
    // Define lang[25][5]={255,0,0,"HALT"} at setup
};

typedef struct {
    int *mem, *stack, *reg;
    int memsize, progsize, stacksize, regcount;
    int pc, sp, tick, tock, retval;
    bool halted, silent;
} VirtualMachine;

static int fieldcount(const char * const csvline)
{
    if (!csvline || *csvline == '\0')
        return 0;
    int fields = 1;
    const char *c = csvline;
    while (*c)
        if (*c++ == ',')
            ++fields;
    return fields;
}

static void * del_vm(VirtualMachine ** vm)
{
    if (vm && *vm) {
        free((*vm)->mem);
        free((*vm)->stack);
        free((*vm)->reg);
        free(*vm);
        *vm = NULL;
    }
    return NULL;
}

static VirtualMachine * new_vm(const char * const csvline)
{
    VirtualMachine *vm = calloc(1, sizeof *vm);  // also resets all vars
    if (!vm)
        return NULL;
    int progsize = fieldcount(csvline);
    int memsize = progsize ? progsize : MEMORYSIZE;
    vm->mem = calloc((size_t)memsize, sizeof *(vm->mem));
    vm->stack = calloc(STACKSIZE, sizeof *(vm->stack));
    vm->reg = calloc(REGISTERS, sizeof *(vm->reg));
    if (!vm->mem || !vm->stack || !vm->reg)
        return del_vm(&vm);
    vm->memsize = memsize;
    vm->stacksize = STACKSIZE;
    vm->regcount = REGISTERS;
    const char *c = csvline;
    while (*c != '\0' && vm->progsize < vm->memsize) {
        int val;
        if (sscanf(c, "%d", &val) != 1)
            return del_vm(&vm);
        vm->mem[vm->progsize++] = val;
        while (*c != '\0' && *c != ',')
            ++c;
        if (*c == ',')
            ++c;
    }
    if (vm->progsize != progsize)
        return del_vm(&vm);
    return vm;
}

static int tick(VirtualMachine * vm)
{
    if (!vm)
        return ERR_NULLPOINTER;
    if (vm->halted)
        return 0;
    if (vm->pc < 0 || vm->pc >= vm->progsize) {
        vm->halted = true;
        return vm->pc < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
    }

    // Start clock cycle
    vm->tick++;

    // Get instruction
    int opcode = vm->mem[vm->pc++];
    if (opcode < OPCODE_MIN || opcode > OPCODE_MAX) {
        vm->halted = true;
        return ERR_INVALID_OPCODE;
    }
    div_t op = div(opcode, 10);
    Instr *instr = &lang[op.quot][op.rem];
    if (instr->opcode != opcode) {
        vm->halted = true;
        return ERR_INVALID_OPCODE;
    }

    // Check stack integrity
    if (instr->stackdir == 1 && vm->sp >= vm->stacksize) {
        vm->halted = true;
        return ERR_STACK_FULL;
    }
    if (instr->stackdir == -1 && vm->sp <= 0) {
        vm->halted = true;
        return ERR_STACK_EMPTY;
    }

    // Get parameters
    int i = 0, par[MAXPARCOUNT] = {0};
    int mode = instr->parmode;
    while (mode) {
        if (i >= MAXPARCOUNT) {
            vm->halted = true;
            return ERR_INTERNAL_PARCOUNT;
        }
        if (vm->pc >= vm->progsize) {
            vm->halted = true;
            return ERR_PC_OVERFLOW;
        }
        int k = vm->mem[vm->pc++];
        switch (mode % 10) {
            case PAR_IMMEDIATE:  // nothing to check for immediate value
                break;
            case PAR_REGISTER:  // check if valid register index
                if (k < 0 || k >= vm->regcount) {
                    vm->halted = true;
                    return ERR_REGISTER_INDEX;
                }
                break;
            case PAR_ABSOLUTE:  // check if valid program address (language only uses addr to jump to, not to store/load)
                if (k < 0 || k >= vm->progsize) {
                    vm->halted = true;
                    return k < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
                }
                break;
            default:
                vm->halted = true;
                return ERR_INTERNAL_PARMODE;  // mode must be IMM/REG/ABS
        }
        par[i++] = k;
        mode /= 10;
    }

    // Execute instruction
    switch (opcode) {
        case 10: vm->reg[par[0]]  = vm->reg[par[1]];    break;  // MOVR Rdst Rsrc
        case 11: vm->reg[par[0]]  = par[1];             break;  // MOVV Rdst val
        case 20: vm->reg[par[0]] += vm->reg[par[1]];    break;  // ADD Rdst Rsrc
        case 21: vm->reg[par[0]] -= vm->reg[par[1]];    break;  // SUB Rdst Rsrc
        case 30: vm->stack[vm->sp++] = vm->reg[par[0]]; break;  // PUSH Rsrc
        case 31: vm->reg[par[0]] = vm->stack[--vm->sp]; break;  // POP Rdst
        case 40: vm->pc = par[0]; break;                        // JP addr
        case 41:                                                // JL Rx Ry addr
            if (vm->reg[par[0]] < vm->reg[par[1]])
                vm->pc = par[2];
            break;
        case 42:                                                // CALL addr (+ push ret-addr)
            vm->stack[vm->sp++] = vm->pc;
            vm->pc = par[0];
            break;
        case 50: vm->pc = vm->stack[--vm->sp]; break;           // RET (+ pop ret-addr)
        case 60:                                                // PRINT Rsrc
            vm->retval = vm->reg[par[0]];
            if (!vm->silent)
                printf("%d\n", vm->retval);
            break;
        case 255: vm->halted = true; break;                     // HALT
        default: vm->halted = true; return ERR_INVALID_OPCODE;  // invalid opcode
    }

    // End clock cycle
    vm->tock++;
    return ERR_OK;
}

static int run(VirtualMachine * vm)
{
    if (!vm)
        return ERR_NULLPOINTER;
    int exitcode = 0;
    while (!vm->halted)
        exitcode = tick(vm);
    return exitcode;
}

int main(void)
{
    lang[25][5] = (Instr){255, 0, 0, "HALT"}; // setup
    VirtualMachine *vm = new_vm(input);       // init
    int exitcode = run(vm);                   // print first 10 Fibonacci numbers
    del_vm(&vm);                              // cleanup
    return exitcode;
}

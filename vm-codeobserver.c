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

// Opcode parameter addressing modes
// #define PAR_IMMEDIATE 1  // unused
#define PAR_REGISTER  2  // parameter is a register index
#define PAR_ABSOLUTE  3  // parameter is an absolute memory address
// #define PAR_RELATIVE  4  // unused

#define ERR_OK                0
#define ERR_NULLPOINTER       1
#define ERR_PC_UNDERFLOW      2
#define ERR_PC_OVERFLOW       3
#define ERR_INVALID_OPCODE    4
#define ERR_STACK_FULL        5
#define ERR_STACK_EMPTY       6
#define ERR_REGISTER_INDEX    7
#define ERR_INTERNAL_PARCOUNT 8

// Program code
static const char *input = "11,0,10,42,6,255,30,0,11,0,0,11,1,1,11,3,1,60,1,10,2,0,20,2,1,60,2,10,0,1,10,1,2,11,2,1,20,3,2,31,2,30,2,41,3,2,19,31,0,50";

// Instruction
// parmode: units = addr mode of 1st par, tens = addr mode of 2nd par, etc.
typedef struct {
    int opcode, parmode;
    char name[8];
} Instr;

static Instr lang[26][6] = {
    {{ 0,  0, "NOP"  }},
    {{10, 22, "MOVR" }, {11,  12, "MOVV"}},
    {{20, 22, "ADD"  }, {21,  22, "SUB" }},
    {{30,  2, "PUSH" }, {31,   2, "POP" }},
    {{40,  3, "JP"   }, {41, 322, "JL"  }, {42, 3, "CALL"}},
    {{50,  0, "RET"  }},
    {{60,  2, "PRINT"}}
    // Define lang[25][5]={255,0,"HALT"} at setup
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
    if (opcode < 0 || opcode > 255)
        return ERR_INVALID_OPCODE;
    div_t op = div(opcode, 10);
    Instr *instr = &lang[op.quot][op.rem];
    if (instr->opcode != opcode)
        return ERR_INVALID_OPCODE;
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
            case PAR_REGISTER:
                if (k < 0 || k >= vm->regcount) {
                    vm->halted = true;
                    return ERR_REGISTER_INDEX;
                }
                break;
            case PAR_ABSOLUTE:
                if (k < 0 || k >= vm->progsize) {
                    vm->halted = true;
                    return k < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
                }
                break;
        }
        par[i++] = k;
        mode /= 10;
    }

    // Execute instruction
    switch (opcode) {
        case 10: // MOVR Rdst Rsrc
            vm->reg[par[0]] = vm->reg[par[1]];
            break;
        case 11: // MOVV Rdst val
            vm->reg[par[0]] = par[1];
            break;
        case 20: // ADD Rdst Rsrc
            vm->reg[par[0]] += vm->reg[par[1]];
            break;
        case 21: // SUB Rdst Rsrc
            vm->reg[par[0]] -= vm->reg[par[1]];
            break;
        case 30: // PUSH Rsrc
            if (vm->sp >= vm->stacksize) {
                vm->halted = true;
                return ERR_STACK_FULL;
            }
            vm->stack[vm->sp++] = vm->reg[par[0]];
            break;
        case 31: // POP Rdst
            if (vm->sp == 0) {
                vm->halted = true;
                return ERR_STACK_EMPTY;
            }
            vm->reg[par[0]] = vm->stack[--vm->sp];
            break;
        case 40: // JP addr
            if (par[0] < 0 || par[0] >= vm->progsize) {
                vm->halted = true;
                return par[0] < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
            }
            vm->pc = par[0];
            break;
        case 41: // JL Rx Ry addr
            if (vm->reg[par[0]] < vm->reg[par[1]]) {
                if (par[2] < 0 || par[2] >= vm->progsize) {
                    vm->halted = true;
                    return par[2] < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
                }
                vm->pc = par[2];
            }
            break;
        case 42: // CALL addr (+ push ret-addr)
            if (vm->sp >= vm->stacksize) {
                vm->halted = true;
                return ERR_STACK_FULL;
            }
            if (par[0] < 0 || par[0] >= vm->progsize) {
                vm->halted = true;
                return par[0] < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
            }
            vm->stack[vm->sp++] = vm->pc;
            vm->pc = par[0];
            break;
        case 50: // RET (+ pop ret-addr)
            if (vm->sp == 0) {
                vm->halted = true;
                return ERR_STACK_EMPTY;
            }
            int addr = vm->stack[--vm->sp];
            if (addr < 0 || addr >= vm->progsize) {
                vm->halted = true;
                return addr < 0 ? ERR_PC_UNDERFLOW : ERR_PC_OVERFLOW;
            }
            vm->pc = addr;
            break;
        case 60: // PRINT Rsrc
            vm->retval = vm->reg[par[0]];
            if (!vm->silent)
                printf("%d\n", vm->retval);
            break;
        case 255: // HALT
            vm->halted = true;
            break;
        default: // invalid opcode
            vm->halted = true;
            return ERR_INVALID_OPCODE;
    }
    // End clock cycle
    vm->tock++;
    return ERR_OK;
}

static int run(VirtualMachine * vm)
{
    if (!vm)
        return -1;
    int exitcode = 0;
    while (!vm->halted)
        exitcode = tick(vm);
    return exitcode;
}

int main(void)
{
    // Setup
    lang[25][5] = (Instr){255, 0, "HALT"};
    // Init
    VirtualMachine *vm = new_vm(input);
    // Run
    int exitcode = run(vm);  // prints first 10 Fibonacci numbers
    // Clean
    del_vm(&vm);
    // Exit
    return exitcode;
}

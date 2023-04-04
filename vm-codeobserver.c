// Puzzle: Implement a fantasy computer to find out the answer to this program
// https://www.reddit.com/r/adventofcode/comments/128t3c6/puzzle_implement_a_fantasy_computer_to_find_out/
// Puzzle designed by https://www.reddit.com/user/codeobserver for https://codeguppy.com
// Solution by E. Dronkert https://github.com/ednl

#include <stdio.h>    // printf, sscanf
#include <stdlib.h>   // malloc, free
#include <stdbool.h>  // bool

#define MEMORYSIZE 64  // actually used by puzzle: 50
#define STACKSIZE  16  // actually used by puzzle: 2
#define REGISTERS   4  // given by puzzle

// Program code
static const char *input = "11,0,10,42,6,255,30,0,11,0,0,11,1,1,11,3,1,60,1,10,2,0,20, 2,1,60,2,10,0,1,10,1,2,11,2,1,20,3,2,31,2,30,2,41,3,2,19,31,0,50";

typedef struct {
    int *mem, *stk, *reg;
    size_t pc, sp, tick, tock;
    size_t memsize, progsize, stksize, regsize;
    int result;
    bool halted, silent;
} VirtualMachine;

static size_t fieldcount(const char * const csvline)
{
    if (!csvline || *csvline == '\0')
        return 0;
    size_t fields = 1;
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
        free((*vm)->stk);
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
    size_t progsize = fieldcount(csvline);
    size_t memsize = progsize ? progsize : MEMORYSIZE;
    vm->mem = calloc(memsize, sizeof *(vm->mem));
    vm->stk = calloc(STACKSIZE, sizeof *(vm->stk));
    vm->reg = calloc(REGISTERS, sizeof *(vm->reg));
    if (!vm->mem || !vm->stk || !vm->reg)
        return del_vm(&vm);
    vm->memsize = memsize;
    vm->stksize = STACKSIZE;
    vm->regsize = REGISTERS;
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
        return -1;  // TODO: define error codes
    if (vm->halted)
        return 0;
    if (vm->pc >= vm->progsize) {
        vm->halted = true;
        return -2;
    }
    // Get instruction
    vm->tick++;
    int opcode = vm->mem[vm->pc++];
    // Get parameters
    int par[3] = {0}, parcount = 0, i = 0;
    switch (opcode) {
        case  41: // JL   (3 params)
            parcount = 3;
            if (vm->pc < vm->progsize)
                par[i++] = vm->mem[vm->pc++];
        case  10: // MOVR (2 params)
        case  11: // MOVV
        case  20: // ADD
        case  21: // SUB
            if (!parcount)
                parcount = 2;
            if (vm->pc < vm->progsize)
                par[i++] = vm->mem[vm->pc++];
        case  30: // PUSH (1 param)
        case  31: // POP
        case  40: // JP
        case  42: // CALL
        case  60: // PRINT
            if (!parcount)
                parcount = 1;
            if (vm->pc < vm->progsize)
                par[i++] = vm->mem[vm->pc++];
        case   0: // NOP  (0 params)
        case  50: // RET
        case 255: // HALT
            break;
        default:  // invalid opcode
            vm->halted = true;
            return -3;
    }
    // Correct number of parameters available?
    if (i != parcount) {
        vm->halted = true;
        return -4;
    }
    // TODO: validate parameters according to addressing mode
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
            if (vm->sp >= vm->stksize) {
                vm->halted = true;
                return -5;
            }
            vm->stk[vm->sp++] = vm->reg[par[0]];
            break;
        case 31: // POP Rdst
            if (vm->sp == 0 || vm->sp > vm->stksize) {
                vm->halted = true;
                return -6;
            }
            vm->reg[par[0]] = vm->stk[--vm->sp];
            break;
        case 40: // JP addr
            if (par[0] < 0 || (size_t)par[0] >= vm->progsize) {
                vm->halted = true;
                return -7;
            }
            vm->pc = (size_t)par[0];
            break;
        case 41: // JL Rx Ry addr
            if (vm->reg[par[0]] < vm->reg[par[1]]) {
                if (par[2] < 0 || (size_t)par[2] >= vm->progsize) {
                    vm->halted = true;
                    return -8;
                }
                vm->pc = (size_t)par[2];
            }
            break;
        case 42: // CALL addr (+ push ret-addr)
            if (vm->sp >= vm->stksize) {
                vm->halted = true;
                return -9;
            }
            if (par[0] < 0 || (size_t)par[0] >= vm->progsize) {
                vm->halted = true;
                return -10;
            }
            vm->stk[vm->sp++] = (int)vm->pc;
            vm->pc = (size_t)par[0];
            break;
        case 50: // RET (+ pop ret-addr)
            if (vm->sp == 0 || vm->sp > vm->stksize) {
                vm->halted = true;
                return -11;
            }
            int addr = vm->stk[--vm->sp];
            if (addr < 0 || (size_t)addr >= vm->progsize) {
                vm->halted = true;
                return -12;
            }
            vm->pc = (size_t)addr;
            break;
        case 60: // PRINT Rsrc
            vm->result = vm->reg[par[0]];
            if (!vm->silent)
                printf("%d\n", vm->result);
            break;
        case 255: // HALT
            vm->halted = true;
            break;
        default: // invalid opcode
            vm->halted = true;
            return -3;
    }
    vm->tock++;
    return 0;
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
    VirtualMachine *vm = new_vm(input);
    int exitcode = run(vm);  // prints first 10 Fibonacci numbers
    del_vm(&vm);
    return exitcode;
}

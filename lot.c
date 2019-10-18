////////// Includes, Defines //////////////////////////////////////////////////

// #ifdef __STDC_ALLOC_LIB__
// #define __STDC_WANT_LIB_EXT2__ 1
// #else
// #define _POSIX_C_SOURCE 200809L
// #endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NAME 64
#define SIZE 250
#define LOOP 2

////////// Globals ////////////////////////////////////////////////////////////

static char *name[SIZE];
static unsigned char ord[SIZE], lot[SIZE], stk[SIZE], sp = 0, size = 0;

////////// Function Declarations //////////////////////////////////////////////

unsigned char swap(unsigned char, unsigned char);
unsigned char push(unsigned char);
unsigned char pop(void);
unsigned char del(unsigned char);
void init(void);
void draw(void);
void print(void);

////////// Function Definitions ///////////////////////////////////////////////

// Swap two elements on the stack
// Arg: index of the elements to swap
// Ret: 1 = ok, 0 = error
unsigned char swap(unsigned char i, unsigned char j)
{
	unsigned char t;

	if (i != j && i < size && j < size) {
		t = stk[i];
		stk[i] = stk[j];
		stk[j] = t;
		return 1;
	}
	return 0;
}

// Push an element onto the stack
// Arg: value to push
// Ret: 1 = ok, 0 = error
unsigned char push(unsigned char n)
{
	if (sp < SIZE) {
		stk[sp++] = n;
		return 1;  // ok
	}
	return 0;  // error
}

// Pop a random element from the stack
// Ret: value of popped element, or 255 = -1 = error
unsigned char pop(void)
{
	unsigned char k;

	if ((k = sp)) {      // check if not zero, remember current stack pointer
		if (--sp)        // adjust stack pointer, check if not zero
			swap(random() % k, sp);  // swap a random element to the top
		return stk[sp];  // return top element
	}
	return 255;  // error
}

// Remove a specific element from the stack
// Arg: value of element to remove
// Ret: value of removed element, or 255 = -1 = error
unsigned char del(unsigned char n)
{
	unsigned char i;

	for (i = 0; i < sp; ++i)     // traverse whole stack
		if (stk[i] == n) {       // if element found
			stk[i] = stk[--sp];  // adjust stack pointer, save (previous) top element
			return n;            // ok
		}
	return 255;  // error
}

// Initialize random number generator and stack
void init(void)
{
	unsigned char i;

	#if defined(__APPLE__) && defined(__MACH__)
	srandomdev();
	#elif defined(__linux__)
	srandom((unsigned int)time(NULL));
	#else
	srand((unsigned int)time(NULL));
	#endif
	for (i = 0; i < SIZE; ++i) {
		name[i] = NULL;
		ord[i] = 0;
		lot[i] = 0;
		stk[i] = 0;
	}
}

// Read data from file, fill stack
unsigned char readfile(void)
{
	unsigned char lines = 0;
	FILE *fp;
	size_t n = 0;

	if ((fp = fopen("lot.txt", "r")) != NULL) {
		while (lines < SIZE && getline(&name[lines], &n, fp) != -1)
			push(lines++);
	}
	return lines;
}

// Pull a straw for everyone
void draw(void)
{
	unsigned char i, loopstart = 0, p, q;

	ord[0] = p = pop();
	for (i = 0; i < SIZE - 1; ++i) {
		if (i - loopstart == LOOP && i <= SIZE - LOOP - 2)
			push(p);
		if (i - loopstart >= LOOP && i == SIZE - LOOP - 2) {
			lot[i] = q = del(p);
		} else
			lot[i] = q = pop();
		if (p == q) {
			ord[i + 1] = p = pop();
			loopstart = i + 1;
		} else
			ord[i + 1] = lot[i];
	}
	lot[SIZE - 1] = p;
}

// Output result
void print(void)
{
	unsigned char i;

	for (i = 0; i < SIZE; ++i)
		printf("%10s -> %-10s\n", name[ord[i]], name[lot[i]]);
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
	unsigned char i;
	init();
	if ((size = readfile())) {
		for (i = 0; i < size; ++i)
			printf("%u %s", i, name[i]);
		//draw();
		//print();
	}
	return 0;
}
